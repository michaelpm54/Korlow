#ifndef EMULATOR_H
#define EMULATOR_H

#include <chrono>
#include <string>
#include <vector>

#include <QMainWindow>
#include <QTimer>

#include "render/opengl.h"

#include "gui/opengl_widget.h"

#include "cpu/cpu.h"
#include "ppu.h"
#include "mmu.h"

#include "gameboy.h"
#include "memory_map.h"
#include "types.h"

class Window;
class FTFont;
class GameboyRenderer;
class MessageManager;
class MessageRenderer;
class MapRenderer;
class QSplitter;
class PpuMapProxy;

class Emulator : public QMainWindow
{
	Q_OBJECT
public:
	Emulator(QWidget *parent = nullptr);
	~Emulator();

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
	void set_map_visible(bool value);

	// Machine
public slots:
	void run();

public:
	void initHardware();

private:
	std::string rom_path;

	std::unique_ptr<FTFont> mFont;

	std::unique_ptr<GameboyRenderer> mGameboyRenderer;
	std::unique_ptr<MessageManager> mMessageManager;
	std::unique_ptr<MessageRenderer> mMessageRenderer;
	std::unique_ptr<MapRenderer> map_renderer;

	QSplitter *splitter;
	OpenGLWidget *main_opengl_widget;
	OpenGLWidget *side_opengl_widget;

	bool mContinue { true };
	bool mPaused { false };
	bool mHaveBios { false };
	bool mHaveRom { false };
	QTimer mFrameTimer;
	std::chrono::time_point<std::chrono::system_clock> mLastDumpTime;

	std::unique_ptr<PpuMapProxy> ppuMapProxy;

	std::unique_ptr<Cpu> cpu;
	std::unique_ptr<Ppu> ppu;
	std::unique_ptr<Mmu> mmu;

	std::unique_ptr<Gameboy> gameboy;
};

#endif // EMULATOR_H
