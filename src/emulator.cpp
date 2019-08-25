#include <cstring>  // strerror
#include <algorithm>
#include <chrono>
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

bool readFileBytes(const std::string &filePath, std::unique_ptr<uint8_t[]> &bytes, int &size);

Emulator::Emulator(std::string biosPath, std::string romPath, uint8_t verbosityFlags)
	:
	mBiosPath(std::move(biosPath)),
	mRomPath(std::move(romPath)),
	mVerbosityFlags(verbosityFlags),
	mMmu(new MMU()),
	mCpu(new CPU()),
	mGpu(new GPU())
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
	const int screenScale = 4;
	mWindow = new Window();
	mWindow->setTitle("Korlow2");
	mWindow->setSize(16*8 * screenScale, 12*8 * screenScale);
	mWindow->create();

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

	// Load ROM
	int romSize;
	if (!readFileBytes(mRomPath, romData, romSize))
	{
		std::cerr << "Failed to open ROM file '" << mRomPath << "'" << std::endl;
	}

	if (romSize > 8388608)
	{
		std::cerr << "ROM file size is too big (> 8 MiB)" << std::endl;
	}

	mMmu->init();
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
