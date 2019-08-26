#include <cstring>  // strerror
#include <algorithm>
#include <iomanip>
#include <locale>
#include <iostream>
#include <memory>
#include <thread>
#include "cpu.h"
#include "emulator.h"
#include "gpu.h"
#include "mmu.h"
#include "window.h"
#include <GLFW/glfw3.h>

bool readFileBytes(const std::string &filePath, std::unique_ptr<uint8_t[]> &bytes, int &size);

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
	const int screenScale = 3;
	mWindow = new Window();
	mWindow->setTitle("Korlow2");
	mWindow->setSize(160 * screenScale, 144 * screenScale);
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
		if (!readFileBytes(mBiosPath, biosData, biosSize))
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
	if (!readFileBytes(mRomPath, romData, romSize))
	{
		std::cerr << "Failed to open ROM file '" << mRomPath << "'" << std::endl;
	}

	if (romSize <= 0x100 || romSize > 8388608)
	{
		std::cerr << "ROM file size is too big (> 8 MiB)" << std::endl;
	}

	mMmu->init();
	mMmu->gpu = mGpu;
	mMmu->bios = biosData.get();
	std::copy_n(romData.get(), romSize, &mMmu->mem[0]);

	mGpu->reset();
	mGpu->mmu = mMmu;

	mCpu->mmu = mMmu;
	mCpu->gpu = mGpu;

	return loop();
}

int Emulator::loop()
{
	while (mContinue && !mWindow->closed() && !mCpu->didBreak())
	{
		mCpu->frame();

		mWindow->events();
		mGpu->frame();
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

	return 0;
}

bool readFileBytes(const std::string &filePath, std::unique_ptr<uint8_t[]> &bytes, int &size)
{
	FILE *file = fopen(filePath.c_str(), "rb");
	if (!file)
	{
		std::cerr << "Failed to open file '" << filePath << "':\n" << std::string(strerror(errno)) << std::endl;
		return false;
	}
	fseek(file, 0, SEEK_END);
	size = ftell(file);
	bytes.reset(new uint8_t[size]);
	fseek(file, 0, SEEK_SET);
	fread(bytes.get(), 1, size, file);
	fclose(file);

	return true;
}

void Emulator::sendKey(int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_D)
		{
			dumpRam();
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
}
