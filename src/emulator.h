#ifndef EMULATOR_H
#define EMULATOR_H

#include <chrono>
#include <string>
#include "key_receiver.h"
#include "memory_map.h"

class Window;
class MMU;
class CPU;
class GPU;
class Font;

class Emulator : public KeyReceiver
{
public:
	Emulator(std::string biosPath, std::string romPath, uint8_t verbosityFlags);
	~Emulator();
	int run();

	virtual void sendKey(int key, int scancode, int action, int mods) override;

private:
	int loop();
	void dumpRam();
	void renderMessages();
	void updateMessages();
	void printRomInfo(romHeader_t &header);

private:
	struct Message
	{
		std::string text;
		float x;
		float y;
		std::chrono::system_clock::time_point begin;
		std::chrono::duration<int, std::milli> timeout;
	};
	
	Window *mWindow { nullptr };
	std::string mBiosPath;
	std::string mRomPath;
	uint8_t mVerbosityFlags;
	MMU *mMmu { nullptr };
	CPU *mCpu { nullptr };
	GPU *mGpu { nullptr };
	bool mContinue { true };
	std::chrono::time_point<std::chrono::system_clock> mLastDumpTime;
	Font *mFont { nullptr };
	std::vector<Message> mMessages;
	bool mPaused { false };
};

#endif // EMULATOR_H
