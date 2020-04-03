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
#include "gpu.h"
#include "message_manager.h"
#include "mmu.h"
#include "rom_util.h"

#include "cpu/cpu.h"
#include "cpu/decode.h"
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
	std::string numWithCommas = std::to_string(mCpu->numInstructionsExecuted());
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
		setRom(FS::readBytes(path));
		mHaveRom = true;
		mGameboyRenderer->setEnabled(true);
	}
	catch (const std::runtime_error& e) {
		std::cerr << e.what() << std::endl;
	}
}

void Emulator::initHardware()
{
	mMmu = std::make_unique<MMU>();
	mCpu = std::make_unique<CPU>(mMmu.get());

	GpuRegisters gpuRegisters =
	{
		.if_ = mMmu->mem[kIf],
		.lcdc = mMmu->mem[kLcdc],
		.stat = mMmu->mem[kStat],
		.scy = mMmu->mem[kScy],
		.scx = mMmu->mem[kScx],
		.ly = mMmu->mem[kLy],
		.lyc = mMmu->mem[kLyc],
		.dmaStartAddr = mMmu->mem[kDmaStartAddr],
		.bgPalette = mMmu->mem[kBgPalette],
		.obj0Palette = mMmu->mem[kObj0Palette],
		.obj1Palette = mMmu->mem[kObj1Palette],
		.windowY = mMmu->mem[kWy],
		.windowX = mMmu->mem[kWx]
	};

	GpuMem gpuMem =
	{
		.oam = &mMmu->mem[kOam],
		.map0 = &mMmu->mem[kMap0],
		.map1 = &mMmu->mem[kMap1],
		.tilesSigned = &mMmu->mem[kTileRamSigned],
		.tilesUnsigned = &mMmu->mem[kTileRamUnsigned],
	};

	mGpu = std::make_unique<GPU>(gpuRegisters, gpuMem, mOpenGLWidget.get());

	mMmu->init(mGpu.get());
	mCpu->mmu = mMmu.get();
	mCpu->gpu = mGpu.get();
}

void Emulator::setBios(const std::string& path)
{
	mMmu->setBios(FS::readBytes(path));
	mHaveBios = true;
}

void Emulator::setRom(const std::vector<uint8_t> &bytes)
{
	mGpu->reset();

	/*
		Special case in case I want to run the boot ROM. :)
		This places some bytes that would usually be there when the user
		has a ROM inserted. The boot ROM expects these bytes.
	*/
	if (bytes.size() == 0x100)
	{
		auto gr { ghostRom() };
		std::memcpy(&mMmu->mem[0x100], gr.data(), gr.size());
	}
	else
	{
		mCpu->reset(mHaveBios);
		printRomInfo(bytes);
	}

	mMmu->setRom(bytes);
}

void Emulator::run()
{
	int dividerCounter = 0;
	int timerCounter = 0;

	while (shouldRun())
	{
		mMessageManager->update();

		if (mPaused)
		{
			mOpenGLWidget->update();
			QApplication::processEvents();
			QApplication::instance()->thread()->msleep(16);
			continue;
		}

		if (mHaveRom)
		{
			int cycles = 0;
			while (cycles < kMaxCyclesPerFrame && !mCpu->paused())
			{
				mCpu->tick(cycles);
				mGpu->tick(cycles);

				dividerCounter += cycles;
				if (dividerCounter >= 256)
				{
					dividerCounter = 0;
					mMmu->mem[0xFF04]++;
				}

				if (mMmu->mem[0xFF07] & 0b0000'0100)
				{
					timerCounter += cycles;
					switch (mMmu->mem[0xFF07] & 0b0000'0011)
					{
					case 0:
						if (timerCounter >= 1024)
						{
							timerCounter = 0;
							mMmu->mem[0xFF05] = mMmu->mem[0xFF06];
							mMmu->write8(kIf, 0b0000'0100);
						}
						break;
					case 1:
						if (timerCounter >= 16)
						{
							timerCounter = 0;
							mMmu->mem[0xFF05] = mMmu->mem[0xFF06];
							//mMmu->write8(kIf, 0b0000'0100);
						}
						break;
					case 2:
						if (timerCounter >= 64)
						{
							timerCounter = 0;
							mMmu->mem[0xFF05] = mMmu->mem[0xFF06];
							mMmu->write8(kIf, 0b0000'0100);
						}
						break;
					case 3:
						if (timerCounter >= 256)
						{
							timerCounter = 0;
							mMmu->mem[0xFF05] = mMmu->mem[0xFF06];
							mMmu->write8(kIf, 0b0000'0100);
						}
						break;
					default:
						break;
					}
				}
			}

			mGpu->updateMap();

			mGameboyRenderer->updatePixels(mGpu->getPixels());
			mGameboyRenderer->updateMap(mGpu->getMap());
		}

		mOpenGLWidget->update();

		QApplication::processEvents();

		QApplication::instance()->thread()->msleep(16);
	}

	printTotalInstructions();
}

bool Emulator::shouldRun() const
{
	return mContinue && isVisible();
}

/*
void Emulator::sendKey(int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_D)
		{
			dumpRam();
		}
		else if (key == GLFW_KEY_M)
		{
			mGpu->updateMap();
		}
		else if (key == GLFW_KEY_SPACE)
		{
			mPaused = !mPaused;
			mMessages.push_back({mPaused ? "Paused" : "Unpaused", 8, 52, std::chrono::system_clock::now(), std::chrono::milliseconds(2000)});
		}
	}
}
*/

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
	fwrite(mMmu->mem.data(), 0x10000, 1, f);
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
		default:
			break;
	}
	mOpenGLWidget->update();
}
