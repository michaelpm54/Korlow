#ifndef EMULATOR_H
#define EMULATOR_H

#include <string>

class Window;
class MMU;
class CPU;
class GPU;

class Emulator
{
public:
	Emulator(std::string biosPath, std::string romPath, uint8_t verbosityFlags);
	~Emulator();
	int run();
	int loop();

private:
	Window *mWindow { nullptr };
	std::string mBiosPath;
	std::string mRomPath;
	uint8_t mVerbosityFlags;
	MMU *mMmu { nullptr };
	CPU *mCpu { nullptr };
	GPU *mGpu { nullptr };
	bool mContinue { true };
};

#endif // EMULATOR_H
