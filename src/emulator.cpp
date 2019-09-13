#include <cstring>  // strerror
#include <algorithm>
#include <iomanip>
#include <locale>
#include <iostream>
#include <memory>
#include <thread>
#include <GLFW/glfw3.h>
#include "cpu.h"
#include "emulator.h"
#include "font.h"
#include "gpu.h"
#include "mmu.h"
#include "window.h"
#include "util.h"

constexpr int kScreenScale = 3;
constexpr int kScreenSizeX = 160 * kScreenScale + (32 * 2 * kScreenScale);
constexpr int kScreenSizeY = 144 * kScreenScale;

Emulator::Emulator(std::string biosPath, std::string romPath, uint8_t verbosityFlags)
	:
	mBiosPath(std::move(biosPath)),
	mRomPath(std::move(romPath)),
	mVerbosityFlags(verbosityFlags),
	mMmu(new MMU()),
	mCpu(new CPU()),
	mGpu(new GPU()),
	mLastDumpTime(std::chrono::system_clock::now() - std::chrono::seconds(2))
{}

Emulator::~Emulator()
{
	delete mGpu;
	delete mMmu;
	delete mCpu;
	delete mWindow;
}

int Emulator::run()
{
	mWindow = new Window();
	mWindow->setTitle("Korlow2");
	mWindow->setSize(kScreenSizeX, kScreenSizeY);
	mWindow->create();
	mWindow->addReceiver(this);

	std::unique_ptr<uint8_t[]> biosData;
	std::unique_ptr<uint8_t[]> romData;

	if (mBiosPath.empty())
	{
		std::cerr << "Please provide a BIOS file." << std::endl;
		return 1;
	}
	else
	{
		int biosSize;
		try
		{
			biosData = readFileBytes(mBiosPath, biosSize);
		}
		catch (const std::runtime_error &e)
		{
			std::cerr << "Failed to open BIOS file '" << mBiosPath << "'" << std::endl;
			return 1;
		}

		if (biosSize != 0x100)
		{
			std::cerr << "BIOS file size is wrong (! 0x100 bytes)" << std::endl;
			return 1;
		}
	}

	int romSize;
	try
	{
		romData = readFileBytes(mRomPath, romSize);
	}
	catch (const std::runtime_error &e)
	{
		std::cerr << "Failed to open ROM file '" << mRomPath << "'" << std::endl;
		return 1;
	}

	if (romSize <= 0x100 || romSize > 8388608)
	{
		std::cerr << "ROM file size is too big (> 8 MiB)" << std::endl;
		return 1;
	}

	mMmu->init();
	mMmu->gpu = mGpu;
	mMmu->bios = biosData.get();
	std::copy_n(romData.get(), romSize, &mMmu->mem[0]);

	mGpu->setSize(kScreenSizeX, kScreenSizeY);
	mGpu->reset();
	mGpu->initOpenGL();
	mGpu->mmu = mMmu;

	mCpu->mmu = mMmu;
	mCpu->gpu = mGpu;

	FT_Init();

	mFont = new Font("../assets/fonts/IBMPlexMono-Semibold.otf");

	return loop();
}

int Emulator::loop()
{
	while (mContinue && !mWindow->closed() && !mCpu->didBreak())
	{

		updateMessages();

		mWindow->events();

		if (!mPaused)
		{
			mCpu->frame();
			mGpu->frame();
		}

		renderMessages();

		mWindow->refresh();

		std::this_thread::sleep_for(std::chrono::milliseconds(16));
	}

	std::string numWithCommas = std::to_string(mCpu->numInstructionsExecuted());
	int insertPosition = numWithCommas.length() - 3;
	while (insertPosition > 0) {
	    numWithCommas.insert(insertPosition, ",");
	    insertPosition-=3;
	}
	std::cerr << "\n" << numWithCommas << " instructions executed" << std::endl;

	delete mFont;

	FT_Done();

	return 0;
}

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
	fwrite(mMmu->mem.get(), 0x10000, 1, f);
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
