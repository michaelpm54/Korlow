#ifndef EMULATOR_H
#define EMULATOR_H

#include <chrono>
#include <string>
#include <vector>

#include <QMainWindow>

#include "opengl.h"

#include "gui/opengl_widget.h"

#include "cpu.h"
#include "gpu.h"
#include "mmu.h"

#include "key_receiver.h"
#include "memory_map.h"

class Window;
class Font;
class GameboyRenderer;
class MessageManager;
class MessageRenderer;

class Emulator : public QMainWindow, public KeyReceiver
{
	Q_OBJECT
public:
	Emulator(QWidget *parent = nullptr);
	~Emulator();

	virtual void sendKey(int key, int scancode, int action, int mods) override;

protected:
	virtual void keyPressEvent(QKeyEvent *event) override;

private:
	// Window
	void setupWindow();
	void createMenuBar();

	// Renderers
	void initGL();

	// Misc
	void dumpRam();
	void printTotalInstructions();
	bool shouldRun() const;
	void openFile(const std::string &path);

	// Machine
	void run();
	void initHardware();
	void setBios(const std::string &path);
	void setRom(const std::vector<std::uint8_t> &bytes);

private:
	std::unique_ptr<GameboyRenderer> mGameboyRenderer;
	std::unique_ptr<MessageManager> mMessageManager;
	std::unique_ptr<MessageRenderer> mMessageRenderer;
	std::unique_ptr<OpenGLWidget> mOpenGLWidget { nullptr };
	std::unique_ptr<Font> mFont;
	std::unique_ptr<CPU> mCpu;
	std::unique_ptr<GPU> mGpu;
	std::unique_ptr<MMU> mMmu;

	std::string mRomPath;
	bool mContinue { true };
	bool mPaused { false };
	bool mHaveBios { false };
	std::chrono::time_point<std::chrono::system_clock> mLastDumpTime;
};

#endif // EMULATOR_H
