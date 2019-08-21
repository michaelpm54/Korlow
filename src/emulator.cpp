#include <cstring>  // strerror
#include <chrono>
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
	// Window setup
	const int screenScale = 3;
	mWindow = new Window();
	mWindow->setTitle("Korlow2");
	mWindow->setSize(160 * screenScale, 144 * screenScale);
	mWindow->create();

	std::unique_ptr<uint8_t[]> biosData;
	std::unique_ptr<uint8_t[]> romData;

	// BIOS state setup
	if (mBiosPath.empty())
	{
		std::cerr << "Please provide a BIOS file." << std::endl;
		return 1;
		mMmu->reset();
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

		mMmu->reset(biosData.get());
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

	mMmu->setBios(biosData.get());
	mMmu->setRom(romData.get(), romSize);

	// Init GPU state
	mGpu->reset();

	mCpu->setMmu(mMmu);

	return loop();
}

int Emulator::loop()
{
	while (mContinue && !mWindow->closed() && !mCpu->didBreak())
	{
		mCpu->frame();

		mWindow->events();
		// mGpu->frame();
		mWindow->refresh();

		std::this_thread::sleep_for(std::chrono::milliseconds(16));
	}

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
