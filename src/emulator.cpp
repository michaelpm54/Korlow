#include <cstring>  // strerror
#include <algorithm>
#include <iomanip>
#include <locale>
#include <iostream>
#include <memory>
#include <thread>

#include <QApplication>
#include <QFileDialog>
#include <QKeyEvent>
#include <QMenuBar>
#include <QThread>

#include "emulator.h"
#include "fs.h"
#include "ppu.h"
#include "message_manager.h"
#include "mmu.h"
#include "rom_util.h"

#include "cpu/cpu.h"
#include "cpu/cpu_instructions.h"

#include "gui/opengl_widget.h"

#include "render/font/ft_font.h"
#include "render/gameboy_renderer.h"
#include "render/message_renderer.h"

Emulator::Emulator(QWidget *parent)
	: QMainWindow(parent)
	, mFont(new FTFont())
	, mLastDumpTime(std::chrono::system_clock::now() - std::chrono::seconds(2))
	, mOpenGLWidget(new OpenGLWidget())
	, mGameboyRenderer(new GameboyRenderer())
	, mMessageManager(new MessageManager())
	, mMessageRenderer(new MessageRenderer(mMessageManager.get(), mFont.get()))
{
	setupWindow();
	createMenuBar();
	show();

	// Important: The renderers are called in order added.
	mOpenGLWidget->addRenderer(mGameboyRenderer.get());
	mOpenGLWidget->addRenderer(mMessageRenderer.get());

	mGameboyRenderer->setEnabled(false);

	setCentralWidget(mOpenGLWidget.get());

	QObject::connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit, this, [this](){ mContinue = false; });

	initHardware();

	//setBios("E:/Projects/Emulators/GB/Korlow/roms/bios.gb");

	initGL();

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
	resize(860, 660);
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

	mOpenGLWidget->makeCurrent();

	mFont->load("E:/Projects/Emulators/GB/Korlow2/assets/fonts/IBMPlexMono-Semibold.otf", 12);

	mMessageRenderer->setFont(mFont.get());
	mGameboyRenderer->initGL();
	mMessageRenderer->initGL();

	mOpenGLWidget->doneCurrent();
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
		mGameboyRenderer->setEnabled(true);
	}
	catch (const std::runtime_error& e) {
		std::cerr << e.what() << std::endl;
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
			mOpenGLWidget->update();
			QApplication::processEvents();
			QApplication::instance()->thread()->msleep(16);
			return;
		}

		if (mHaveRom)
		{
			gameboy->tick();

			mGameboyRenderer->updatePixels(ppu->get_pixels());
			mGameboyRenderer->updateMap(ppuMapProxy->get_pixels());
		}

		mOpenGLWidget->update();
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
	auto now = std::chrono::system_clock::now();
	if (now - mLastDumpTime < std::chrono::seconds(2))
	{
		std::cout << "Tried to dump RAM too soon after last time" << std::endl;
		return;
	}
	std::cout << "Dumping RAM... ";
	mLastDumpTime = now;

	FILE *f = fopen("ramdump.bin", "wb+");
	if (!f)
	{
		std::cerr << "Failed to open ramdump.bin" << std::endl;
		return;
	}
	fwrite(gameboy->get_memory(), 0x10000, 1, f);
	fclose(f);
	std::cout << "Dumped RAM" << std::endl;

	std::string text = "Dumped RAM to file: 'ramdump.bin'";
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
		default:
			break;
	}
	mOpenGLWidget->update();
}
