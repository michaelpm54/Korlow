#ifndef EMULATOR_H
#define EMULATOR_H

#include "opengl.h"

#include <chrono>
#include <string>
#include <vector>

#include <QApplication>

#include "gui/main_window.h"
#include "gui/opengl_widget.h"

#include "cpu.h"
#include "gpu.h"
#include "mmu.h"

#include "key_receiver.h"
#include "memory_map.h"

class Window;
class Font;

class Emulator : public QApplication, public KeyReceiver
{
	Q_OBJECT
public:
	Emulator(int argc, char *argv[]);
	//Emulator(std::string romPath, uint8_t verbosityFlags);
	void run();

	virtual void sendKey(int key, int scancode, int action, int mods) override;

private:
	int loop();
	void dumpRam();
	void renderMessages();
	void updateMessages();
	void printRomInfo(romHeader_t &header);

private slots:
	void openFile(const std::string &path);

private:
	void initHardware();
	void setBios(const std::string &path);
	void setRom(const std::vector<std::uint8_t> &bytes);

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
	std::string mRomPath;
	uint8_t mVerbosityFlags;
	bool mContinue { true };
	std::chrono::time_point<std::chrono::system_clock> mLastDumpTime;
	Font *mFont { nullptr };
	std::vector<Message> mMessages;
	bool mPaused { false };

	std::unique_ptr<MMU> mMmu;
	std::unique_ptr<CPU> mCpu;
	std::unique_ptr<GPU> mGpu;

	std::unique_ptr<MainWindow> mMainWindow { nullptr };
	std::unique_ptr<OpenGLWidget> mOpenGLWidget { nullptr };
};

#endif // EMULATOR_H
