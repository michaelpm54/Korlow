#include <cstring>  // strerror
#include <algorithm>
#include <iomanip>
#include <locale>
#include <iostream>
#include <memory>
#include <thread>

#include "cpu.h"
#include "emulator.h"
#include "font.h"
#include "fs.h"
#include "gpu.h"
#include "mmu.h"
#include "window.h"

#include "gui/main_window.h"
#include "gui/opengl_widget.h"

Emulator::Emulator(int argc, char *argv[])
	: QApplication(argc, argv)
	, mMainWindow(new MainWindow())
	, mOpenGLWidget(new OpenGLWidget())
{
	mMainWindow->setCentralWidget(mOpenGLWidget.get());

	connect(this, &QCoreApplication::aboutToQuit, this, [this](){ mContinue = false; });
	connect(mMainWindow.get(), &MainWindow::openFile, this, &Emulator::openFile);

	initHardware();
	setBios("E:/Projects/Emulators/GB/Korlow/roms/bios.gb");
}

void Emulator::openFile(const std::string& path)
{
	if (path.empty()) { return; }

	try {
		setRom(FS::readBytes(path));
	}
	catch (const std::runtime_error& e) {
		std::cerr << e.what() << std::endl;
	}
}

void Emulator::initHardware()
{
	mMmu = std::make_unique<MMU>();
	mCpu = std::make_unique<CPU>();

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
	
}

void Emulator::setRom(const std::vector<uint8_t> &bytes)
{
	romHeader_t romHeader;
	std::memcpy(&romHeader, &bytes[0x100], sizeof(romHeader_t));
	printRomInfo(romHeader);

	mMmu->setRom(bytes);

	mGpu->reset();

	mCpu->initWithoutBios();

	//FT_Init();

	//mFont = new Font("E:/Projects/Emulators/GB/Korlow2/assets/fonts/IBMPlexMono-Semibold.otf");

	loop();

	QCoreApplication::quit();
}

/*
Emulator::Emulator(std::string romPath, uint8_t verbosityFlags)
	:
	QApplication(1, nullptr),
	mRomPath(std::move(romPath)),
	mVerbosityFlags(verbosityFlags),
	mMmu(new MMU()),
	mCpu(new CPU()),
	mGpu(new GPU()),
	mLastDumpTime(std::chrono::system_clock::now() - std::chrono::seconds(2))
{}
*/

void Emulator::run()
{
	exec();
/*
	mWindow = new Window();
	mWindow->setTitle("Korlow2");
	mWindow->setSize(kScreenSizeX, kScreenSizeY);
	mWindow->create();
	mWindow->addReceiver(this);

	std::unique_ptr<uint8_t[]> romData;

	int romSize;
	try
	{
		romData = readFileBytes(mRomPath, romSize);
	}
	catch (const std::runtime_error &)
	{
		std::cerr << "Failed to open ROM file '" << mRomPath << "'" << std::endl;
		return 1;
	}

	if (romSize > 8388608)
	{
		std::cerr << "ROM file size is too big (> 8 MiB)" << std::endl;
		return 1;
	}

	romHeader_t romHeader;
	std::memcpy(&romHeader, &romData.get()[0x100], sizeof(romHeader_t));
	printRomInfo(romHeader);

	mMmu->init();
	mMmu->gpu = mGpu;
	mMmu->set_rom(romData.get(), romSize);

	mGpu->setSize(kScreenSizeX, kScreenSizeY);
	mGpu->reset();
	mGpu->initOpenGL();
	mGpu->mmu = mMmu;

	mCpu->mmu = mMmu;
	mCpu->gpu = mGpu;

	// Special case in case I want to run the boot ROM :)
	if (romSize == 0x100)
	{
		constexpr uint8_t logo[] =
		{
			0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B,
			0x03, 0x73, 0x00, 0x83, 0x00, 0x0C, 0x00, 0x0D,
			0x00, 0x08, 0x11, 0x1F, 0x88, 0x89, 0x00, 0x0E,
			0xDC, 0xCC, 0x6E, 0xE6, 0xDD, 0xDD, 0xD9, 0x99,
			0xBB, 0xBB, 0x67, 0x63, 0x6E, 0x0E, 0xEC, 0xCC,
			0xDD, 0xDC, 0x99, 0x9F, 0xBB, 0xB9, 0x33, 0x3E
		};
		std::memcpy(&mMmu->mem[0x104], logo, sizeof(logo));
		// a = 0x19 at this point
		// a += mem[0x14D] needs to be equal to 0
		// 0x19 + 0xE7 = 0x100 or 0x00 as a byte
		mMmu->write8(0x14D, 0xE7);
		// jr -2
		mMmu->write8(0x100, 0x18);
		mMmu->write16(0x101, 0x00FE);
	}
	else
	{
		mCpu->initWithoutBios();
	}

	FT_Init();

	mFont = new Font("../assets/fonts/IBMPlexMono-Semibold.otf");

	return loop();
*/
}

int Emulator::loop()
{
	while (mContinue && /*!mWindow->closed() &&*/ !mCpu->didBreak() && mMainWindow->isVisible())
	{
		//updateMessages();

		if (!mPaused)
		{
			mCpu->frame();
			mGpu->updateMap();
			mOpenGLWidget->update(mGpu->getPixels());
		}

		//renderMessages();

		std::this_thread::sleep_for(std::chrono::milliseconds(16));

		processEvents();
	}

	std::string numWithCommas = std::to_string(mCpu->numInstructionsExecuted());
	int insertPosition = numWithCommas.length() - 3;
	while (insertPosition > 0) {
	    numWithCommas.insert(insertPosition, ",");
	    insertPosition-=3;
	}
	std::cerr << "\n" << numWithCommas << " instructions executed" << std::endl;

	//delete mFont;

	//FT_Done();

	return 0;
}

void Emulator::sendKey(int key, int scancode, int action, int mods)
{
	/*
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
	*/
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
	fwrite(mMmu->mem.data(), 0x10000, 1, f);
	fclose(f);
	std::cout << "Dumped RAM" << std::endl;

	std::string text = "Dumped RAM to file: 'ramdump.bin'";
	mMessages.push_back({text, 8, 52, now, std::chrono::milliseconds(2000)});
}

void Emulator::renderMessages()
{
	for (const auto &msg : mMessages)
	{
		mFont->renderText(msg.text, msg.x, msg.y);
	}
}

void Emulator::updateMessages()
{
	auto now = std::chrono::system_clock::now();
	mMessages.erase(
		std::remove_if(
			mMessages.begin(),
			mMessages.end(),
			[=](auto &m)
			{
				return std::chrono::duration_cast<std::chrono::milliseconds>(now - m.begin) > m.timeout;
			}
		),
		mMessages.end()
	);
}

void Emulator::printRomInfo(romHeader_t &header)
{
	printf("Title: %s\n", header.title);
	printf("ROM Size: %d\n", (2 << 14) << header.romSize);
	printf("Cart type: %s\n", kCartTypes.at(header.cartType));
}
