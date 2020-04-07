#include "render/opengl.h"

#include <algorithm>
#include <iomanip>
#include <fstream>
#include <locale>
#include <iostream>
#include <memory>
#include <thread>

#include <QOffscreenSurface>
#include <QApplication>
#include <QMessageBox>
#include <QFileDialog>
#include <QFrame>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QMenuBar>
#include <QThread>

#include "constants.h"
#include "render/gl_util.h"
#include "emulator.h"
#include "fs.h"
#include "ppu.h"
#include "ppu_map_proxy.h"
#include "message_manager.h"
#include "mmu.h"
#include "rom_util.h"

#include "cpu/cpu.h"
#include "cpu/cpu_instructions.h"

#include "gui/opengl_widget.h"

#include "render/font/ft_font.h"
#include "render/map_scene.h"
#include "render/message_renderer.h"
#include "render/main_scene.h"

Emulator::Emulator(QWidget *parent)
	: QMainWindow(parent)
	, last_dump_time(std::chrono::system_clock::now() - std::chrono::seconds(2))
	, main_scene(new MainScene())
	, map_scene(new MapScene())
{
	try {
		setup();
	} catch (const std::runtime_error& e) {
		QMessageBox::warning(this, "Failed during setup", QString::fromStdString(e.what()));
	}
}

/* NOTE: Keep this destructor to make unique_ptr's work. */
Emulator::~Emulator() {}

void Emulator::setup()
{
	setup_window();
	setup_menu_bar();
	setup_widgets();

	show();
	QApplication::processEvents();

	set_have_rom(false);
	set_map_visible(false);

	setup_slots();
	setup_gameboy_components();

	frame_timer.start((1.0f / 60.0f) * 1000.0f);
}

void Emulator::setup_slots()
{
	connect(&frame_timer, &QTimer::timeout, this, &Emulator::slot_tick);

	connect(action_open, &QAction::triggered, this, &Emulator::slot_open);
	connect(action_close, &QAction::triggered, this, &Emulator::slot_close);
	connect(action_quit, &QAction::triggered, this, &Emulator::slot_quit);
}

void Emulator::setup_widgets()
{
	main_scene->setFixedSize({kLcdWidth * kScale, kLcdHeight * kScale});
	map_scene->setFixedSize({kMapWidth * kScale, kLcdHeight * kScale});

	auto central_frame { new QFrame() };
	central_frame->setFrameStyle(QFrame::NoFrame);
	central_frame->setLayout(new QHBoxLayout());
	auto layout { central_frame->layout() };
	layout->addWidget(main_scene);
	layout->addWidget(map_scene);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(0);
	setCentralWidget(central_frame);
}

void Emulator::setup_window()
{
	setMinimumSize(100, 100);
	setFocusPolicy(Qt::FocusPolicy::StrongFocus);
	setAttribute(Qt::WA_QuitOnClose);
	resize(kLcdWidth * kScale, kLcdHeight * kScale);
	setWindowTitle("[AETHIC]");
	setWindowFlags(
		Qt::CustomizeWindowHint |
		Qt::WindowTitleHint |
		Qt::WindowMinimizeButtonHint |
		Qt::WindowSystemMenuHint |
		Qt::WindowCloseButtonHint
	);
}

void Emulator::setup_menu_bar()
{
	action_open = new QAction("&Open");
	action_close = new QAction("&Close");
	action_quit = new QAction("&Quit");

	action_open->setShortcut(QKeySequence(Qt::Modifier::CTRL + Qt::Key::Key_O));
	action_close->setShortcut(QKeySequence(Qt::Modifier::CTRL + Qt::Key::Key_W));
	action_quit->setShortcut(QKeySequence(Qt::Modifier::CTRL + Qt::Key::Key_Q));

	auto file_menu { new QMenu("&File") };
	file_menu->addAction(action_open);
	file_menu->addAction(action_close);
	file_menu->addSeparator();
	file_menu->addAction(action_quit);

	menuBar()->addMenu(file_menu);
}

//void Emulator::setup_opengl_resources()
//{
	/*
	if (!main_opengl_widget->context())
		throw std::runtime_error("OpenGL context doesn't exist");

	font->load("E:/Projects/Emulators/GB/Korlow2/assets/fonts/IBMPlexMono-Semibold.otf", 12);
	message_renderer->set_font(font.get());
	main_opengl_widget->doneCurrent();

	side_opengl_widget->makeCurrent();

	if (!main_opengl_widget->context())
		throw std::runtime_error("OpenGL context doesn't exist");

	map_renderer->setup_opengl_resources();
	main_opengl_widget->doneCurrent();
	*/
//}

void Emulator::print_total_instructions()
{
	std::string numWithCommas = std::to_string(0); // FIXME
	int insertPosition { static_cast<int>(numWithCommas.length() - 3) };
	while (insertPosition > 0) {
		numWithCommas.insert(insertPosition, ",");
		insertPosition -= 3;
	}
	printf("\n%s instructions executed.\n", numWithCommas.c_str());
}

void Emulator::setup_gameboy_components()
{
	gameboy = std::make_unique<Gameboy>();

	uint8_t *mem { gameboy->get_memory() };

	cpu = std::make_unique<Cpu>(
		CpuRegisters {
			.io  = mem[kIo],
			.if_ = mem[kIf],
			.ie  = mem[kIe],
		}
	);

	ppu = std::make_unique<Ppu>(
		PpuRegisters {
			.if_  = mem[kIf],
			.lcdc = mem[kLcdc],
			.stat = mem[kStat],
			.scx  = mem[kScx],
			.scy  = mem[kScy],
			.ly   = mem[kLy],
			.lyc  = mem[kLyc],
			.wy   = mem[kWy],
			.wx   = mem[kWx],
		}
	);

	ppu_proxy = std::make_unique<PpuMapProxy>(*ppu);

	mmu = std::make_unique<Mmu>(*cpu, *ppu_proxy, gameboy->get_memory());

	gameboy->set_components(*cpu, *ppu, *mmu);
}

void Emulator::update()
{
	if (config.paused || !have_rom)
		return;

	gameboy->tick();

}

void Emulator::slot_tick()
{
	update();
	main_scene->update(gameboy.get());
	map_scene->update(ppu_proxy.get());
	QApplication::processEvents();
}

void Emulator::dump_memory()
{
	if (!have_rom)
	{
		return;
	}

	auto now = std::chrono::system_clock::now();
	if (now - last_dump_time < std::chrono::seconds(2))
	{
		return;
	}

	last_dump_time = now;

	auto filename { config.rom_path + ".dump" };

	try {
		FS::write_bytes(filename, gameboy->get_memory(), 0x10000);
	}
	catch (const std::runtime_error& e) {
		QMessageBox::warning(this, "Failed to dump memory", QString::fromStdString(e.what()));
	}

	std::string text = "Dumped RAM to file: " + filename;
	//message_manager->addMessage(text, 8, 40, 2000);
}

void Emulator::keyPressEvent(QKeyEvent* event)
{
	switch (event->key())
	{
		case Qt::Key_Space:
			config.paused = !config.paused;
			//message_manager->addMessage(paused ? "Paused" : "Unpaused", 8, 40, 2000);
			break;
		case Qt::Key_D:
			if (event->modifiers() & Qt::ShiftModifier)
			{
				cpu->debug = !cpu->debug;
				//message_manager->addMessage(cpu->debug ? "Debug enabled" : "Debug disabled", 8, 40, 2000);
			}
			else
			{
				dump_memory();
			}
			break;
		case Qt::Key_M:
			set_map_visible(!map_scene->isVisible());
			break;
		case Qt::Key_Right:
			mmu->write8(kScx, mmu->read8(kScx) + 1);
			break;
		case Qt::Key_Left:
			mmu->write8(kScx, mmu->read8(kScx) - 1);
			break;
		case Qt::Key_Up:
			mmu->write8(kScy, mmu->read8(kScy) - 1);
			break;
		case Qt::Key_Down:
			mmu->write8(kScy, mmu->read8(kScy) + 1);
			break;
		default:
			break;
	}
	ppu->refresh();
}

void Emulator::set_map_visible(bool value)
{
	config.map_visible = value;
	map_scene->setVisible(value);

	QSize size;

	if (value)
		size = {(kLcdWidth + kMapWidth) * kScale, kLcdHeight * kScale};
	else
		size = {kLcdWidth * kScale, kLcdHeight * kScale};

	resize(size);
}

void Emulator::slot_open()
{
	const QString caption { "Open File" };
	const QString current { QDir::currentPath() };
	const QString filter { "ROM (*.gb *.rom *.bin *.dump)" };

	auto path { QFileDialog::getOpenFileName(this, caption, current, filter).toStdString() };

	if (path.empty()) { return; }

	gameboy->reset();

	try {
		gameboy->set_rom(FS::read_bytes(path));
	}
	catch (const std::runtime_error& e) {
		QMessageBox::warning(this, "Failed to open ROM", QString::fromStdString(e.what()));
	}

	config.rom_path = path;
	
	set_have_rom(true);
}

void Emulator::slot_close()
{
	set_have_rom(false);
}

void Emulator::slot_quit()
{
	slot_close();
	QApplication::quit();
}

void Emulator::set_have_rom(bool value)
{
	have_rom = value;
	main_scene->set_have_rom(value);
	map_scene->set_have_rom(value);
}
