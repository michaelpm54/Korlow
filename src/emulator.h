#ifndef EMULATOR_H
#define EMULATOR_H

#include "render/opengl.h"

#include <chrono>
#include <string>
#include <vector>

#include <QMainWindow>
#include <QOpenGLContext>
#include <QTimer>

#include "gui/opengl_widget.h"

#include "cpu/cpu.h"
#include "ppu.h"
#include "mmu.h"

#include "gameboy.h"
#include "memory_map.h"
#include "types.h"
#include "emu_types.h"

class FTFont;
class MainScene;
class MapScene;
class MessageManager;
class MessageRenderer;
class MapRenderer;
struct PpuMapProxy;

struct Config
{
	bool map_visible { false };
	bool paused { false };
	std::string rom_path;
};

class Emulator : public QMainWindow
{
	Q_OBJECT
public:
	Emulator(std::function <void (const std::vector<u8> &data, QMainWindow &main_window)> open_rom_callback, QWidget *parent = nullptr);

	/* NOTE: Keep this destructor to make unique_ptr's work. */
	~Emulator();

protected:
	virtual void keyPressEvent(QKeyEvent *event) override;

private:
	void setup_window();
	void setup_menu_bar();
	void setup_widgets();
	void setup_slots();
	void setup_gameboy_components();
	
	void dump_memory();
	void print_total_instructions();
	void set_map_visible(bool value);

	void update();

	void set_have_rom(bool value);

public slots:
	void slot_tick();

	void slot_open();
	void slot_close();
	void slot_quit();

private:
	Config config;

	std::unique_ptr<FTFont> font;

	MainScene *main_scene;
	MapScene *map_scene;

	bool have_rom { false };

	QTimer frame_timer;
	std::chrono::time_point<std::chrono::system_clock> last_dump_time;

	std::unique_ptr<PpuMapProxy> ppu_proxy;

	std::unique_ptr<Cpu> cpu;
	std::unique_ptr<Ppu> ppu;
	std::unique_ptr<Mmu> mmu;

	std::unique_ptr<Gameboy> gameboy;

	QAction *action_open;
	QAction *action_close;
	QAction *action_quit;

	std::function<void (const std::vector<u8> &data, QMainWindow &main_window)> m_open_rom_callback;
};

#endif // EMULATOR_H
