#include <algorithm>
#include <iomanip>
#include <fstream>
#include <locale>
#include <iostream>
#include <memory>
#include <thread>

#include <QApplication>
#include <QMessageBox>
#include <QFileDialog>
#include <QFrame>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QMenuBar>
#include <QSplitter>
#include <QThread>

#include "emulator.h"
#include "fs.h"
#include "ppu.h"
#include "ppu_map_proxy.h"
#include "message_manager.h"
#include "mmu.h"
#include "rom_util.h"

#include "cpu/cpu.h"
#include "cpu/cpu_instructions.h"

#include "gui/opengl_widget.h"

#include "render/font/ft_font.h"
#include "render/gameboy_renderer.h"
#include "render/map_renderer.h"
#include "render/message_renderer.h"

Emulator::Emulator(QWidget *parent)
	: QMainWindow(parent)
	, mFont(new FTFont())
	, mLastDumpTime(std::chrono::system_clock::now() - std::chrono::seconds(2))
	, splitter(new QSplitter(Qt::Horizontal))
	, main_opengl_widget(new OpenGLWidget())
	, side_opengl_widget(new OpenGLWidget())
	, mGameboyRenderer(new GameboyRenderer())
	, mMessageManager(new MessageManager())
	, mMessageRenderer(new MessageRenderer(mMessageManager.get(), mFont.get()))
	, map_renderer(new MapRenderer())
{
	setupWindow();
	createMenuBar();
	show();

	// Important: The renderers are called in order added.
	main_opengl_widget->addRenderer(mGameboyRenderer.get());
	main_opengl_widget->addRenderer(mMessageRenderer.get());
	main_opengl_widget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	main_opengl_widget->setFixedSize({160 * 4, 144 * 4});

	side_opengl_widget->addRenderer(map_renderer.get());

	mGameboyRenderer->setEnabled(false);

	splitter->addWidget(main_opengl_widget);
	splitter->addWidget(side_opengl_widget);
	splitter->setChildrenCollapsible(false);
	splitter->setHandleWidth(0);

	setCentralWidget(splitter);

	QObject::connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit, this, [this](){ mContinue = false; });

	initHardware();

	//setBios("E:/Projects/Emulators/GB/Korlow/roms/bios.gb");

	try {
		initGL();
	} catch (const std::runtime_error& e) {
		QMessageBox::warning(this, "Failed to init resources", QString::fromStdString(e.what()));
	}

	set_map_visible(false);

	QObject::connect(&mFrameTimer, &QTimer::timeout, this, &Emulator::run);
	mFrameTimer.setInterval((1.0f / 60.0f) * 1000.0f);
	mFrameTimer.start();
}

Emulator::~Emulator()
{
	QApplication::quit();
}

void Emulator::setupWindow()
{
	setFocusPolicy(Qt::FocusPolicy::StrongFocus);
	setAttribute(Qt::WA_QuitOnClose);
	resize(160 * 4, 144 * 4);
	setWindowTitle("Korlow");
}

void Emulator::createMenuBar()
{
	QMenuBar *mb { menuBar() };

	QMenu *fileMenu(new QMenu("&File"));

	QAction *openAction(new QAction("&Open"));
	openAction->setShortcut(QKeySequence(Qt::Modifier::CTRL + Qt::Key::Key_O));
	connect(openAction, &QAction::triggered, this, [this]()
	{
		openFile(QFileDialog::getOpenFileName(
			this,
			"Open File",
			QDir::currentPath(),
			"ROM (*.gb *.rom *.bin)"
		).toStdString());
	}
	);
	fileMenu->addAction(openAction);

	QAction *closeAction(new QAction("&Close"));
	closeAction->setShortcut(QKeySequence(Qt::Modifier::CTRL + Qt::Key::Key_W));
	connect(closeAction, &QAction::triggered, this, [this]()
	{
		mHaveRom = false;
		mGameboyRenderer->setEnabled(false);
	}
	);
	fileMenu->addAction(closeAction);

	fileMenu->addSeparator();

	QAction *quitAction(new QAction("&Quit"));
	quitAction->setShortcut(QKeySequence(Qt::Modifier::CTRL + Qt::Key::Key_Q));
	connect(quitAction, &QAction::triggered, this, [this](){ close(); });
	fileMenu->addAction(quitAction);

	mb->addMenu(fileMenu);
}

void Emulator::initGL()
{
	QApplication::processEvents();

	main_opengl_widget->makeCurrent();

	mFont->load("E:/Projects/Emulators/GB/Korlow2/assets/fonts/IBMPlexMono-Semibold.otf", 12);

	mMessageRenderer->setFont(mFont.get());
	mGameboyRenderer->initGL();
	mMessageRenderer->initGL();

	main_opengl_widget->doneCurrent();

	side_opengl_widget->makeCurrent();
	map_renderer->initGL();
	main_opengl_widget->doneCurrent();
}

void Emulator::printTotalInstructions()
{
	std::string numWithCommas = std::to_string(0); // FIXME
	int insertPosition { static_cast<int>(numWithCommas.length() - 3) };
	while (insertPosition > 0) {
		numWithCommas.insert(insertPosition, ",");
		insertPosition-=3;
	}
	printf("\n%s instructions executed.\n", numWithCommas.c_str());
}

void Emulator::openFile(const std::string& path)
{
	if (path.empty()) { return; }

	try {
		gameboy->set_rom(FS::readBytes(path));
		mHaveRom = true;
		rom_path = path;
		mGameboyRenderer->setEnabled(true);
	}
	catch (const std::runtime_error& e) {
		QMessageBox::warning(this, "Failed to open ROM", QString::fromStdString(e.what()));
	}
}

void Emulator::initHardware()
{
	{
		gameboy = std::make_unique<Gameboy>();

		uint8_t *mem { gameboy->get_memory() };

		cpu = std::make_unique<Cpu>(
			CpuRegisters {
				.io  = mem[kIo],
				.if_ = mem[kIf],
				.ie  = mem[kIe],
			}
		);
		ppu = std::make_unique<Ppu>(
			PpuRegisters {
				.if_  = mem[kIf],
				.lcdc = mem[kLcdc],
				.stat = mem[kStat],
				.scx  = mem[kScx],
				.scy  = mem[kScy],
				.ly   = mem[kLy],
				.lyc  = mem[kLyc],
				.wy   = mem[kWy],
				.wx   = mem[kWx],
			}
		);
		ppuMapProxy = std::make_unique<PpuMapProxy>(*ppu);
		mmu = std::make_unique<Mmu>(*cpu, *ppuMapProxy, gameboy->get_memory());

		gameboy->set_components(*cpu, *ppu, *mmu);
		gameboy->reset();
		return;
	}
}

void Emulator::run()
{
	if (gameboy->is_running())
	{
		mMessageManager->update();

		if (mPaused)
		{
			main_opengl_widget->update();
			QApplication::processEvents();
			QApplication::instance()->thread()->msleep(16);
			return;
		}

		if (mHaveRom)
		{
			gameboy->tick();

			mGameboyRenderer->update(ppu->get_pixels());
			map_renderer->update(ppuMapProxy->get_pixels());
		}

		main_opengl_widget->update();
		side_opengl_widget->update();
		QApplication::processEvents();
		QApplication::instance()->thread()->msleep(16);
	}
}

bool Emulator::shouldRun() const
{
	return mContinue && isVisible();
}

void Emulator::dumpRam()
{
	if (!mHaveRom)
		return;

	auto now = std::chrono::system_clock::now();
	if (now - mLastDumpTime < std::chrono::seconds(2))
	{
		return;
	}
	mLastDumpTime = now;

	auto filename { rom_path + ".dump" };

	try {
		FS::writeBytes(filename, gameboy->get_memory(), 0x10000);
	}
	catch (const std::runtime_error& e) {
		QMessageBox::warning(this, "Failed to dump memory", QString::fromStdString(e.what()));
	}

	std::string text = "Dumped RAM to file: " + filename;
	mMessageManager->addMessage(text, 8, 40, 2000);
}

void Emulator::keyPressEvent(QKeyEvent* event)
{
	switch (event->key())
	{
		case Qt::Key_Space:
			mPaused = !mPaused;
			mMessageManager->addMessage(mPaused ? "Paused" : "Unpaused", 8, 40, 2000);
			break;
		case Qt::Key_D:
			if (event->modifiers() & Qt::ShiftModifier)
			{
				cpu->debug = !cpu->debug;
				mMessageManager->addMessage(cpu->debug ? "Debug enabled" : "Debug disabled", 8, 40, 2000);
			}
			else
				dumpRam();
			break;
		case Qt::Key_M:
			set_map_visible(!side_opengl_widget->isVisible());
			break;
		case Qt::Key_Right:
			mmu->write8(kScx, mmu->read8(kScx) + 1);
			break;
		case Qt::Key_Left:
			mmu->write8(kScx, mmu->read8(kScx) - 1);
			break;
		case Qt::Key_Up:
			mmu->write8(kScy, mmu->read8(kScy) - 1);
			break;
		case Qt::Key_Down:
			mmu->write8(kScy, mmu->read8(kScy) + 1);
			break;
		default:
			break;
	}
	main_opengl_widget->update();
}

void Emulator::set_map_visible(bool value)
{
	if (value)
	{
		resize(192 * 4, 144 * 4);
		side_opengl_widget->show();
	}
	else
	{
		resize(160 * 4, 144 * 4);
		side_opengl_widget->hide();
	}
}
