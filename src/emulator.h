#ifndef EMULATOR_H
#define EMULATOR_H

#include <chrono>
#include <string>
#include "key_receiver.h"

class Window;
class MMU;
class CPU;
class GPU;

class Emulator : public KeyReceiver
{
public:
	Emulator(std::string biosPath, std::string romPath, uint8_t verbosityFlags);
	~Emulator();
	int run();
	int loop();

	virtual void sendKey(int key, int scancode, int action, int mods) override;

	void dumpRam();

private:
	Window *mWindow { nullptr };
	std::string mBiosPath;
	std::string mRomPath;
	uint8_t mVerbosityFlags;
	MMU *mMmu { nullptr };
	CPU *mCpu { nullptr };
	GPU *mGpu { nullptr };
	bool mContinue { true };
	std::chrono::time_point<std::chrono::system_clock> mLastDumpTime;
};

#endif // EMULATOR_H
