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

Emulator::Emulator(std::function <void(const std::vector<u8>& data, QMainWindow& main_window)> open_rom_callback, QWidget* parent)
	: QMainWindow(parent)
	, last_dump_time(std::chrono::system_clock::now() - std::chrono::seconds(2))
	// , main_scene(new MainScene())
	// , map_scene(new MapScene())
	, m_open_rom_callback(open_rom_callback)
{
	try {
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
	catch (const std::runtime_error& e) {
		QMessageBox::warning(this, "Failed during setup", QString::fromStdString(e.what()));
	}
}

/* NOTE: Keep this destructor to make unique_ptr's work. */
Emulator::~Emulator() {
}

void Emulator::setup_slots()
{
	// connect(&frame_timer, &QTimer::timeout, this, &Emulator::slot_tick);

	connect(action_open, &QAction::triggered, this, &Emulator::slot_open);
	connect(action_close, &QAction::triggered, this, &Emulator::slot_close);
	connect(action_quit, &QAction::triggered, this, &Emulator::slot_quit);
}

void Emulator::setup_widgets()
{
	/*
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
	*/
}

void Emulator::setup_window()
{
	setMinimumSize(100, 100);
	setFocusPolicy(Qt::FocusPolicy::StrongFocus);
	setAttribute(Qt::WA_QuitOnClose);
	resize(kLcdWidth * kScale, kLcdHeight * kScale);
	setWindowTitle("DMG Emulator");
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

	auto file_menu{ new QMenu("&File") };
	file_menu->addAction(action_open);
	file_menu->addAction(action_close);
	file_menu->addSeparator();
	file_menu->addAction(action_quit);

	menuBar()->addMenu(file_menu);
}

void Emulator::print_total_instructions()
{
	std::string numWithCommas = std::to_string(0); // FIXME
	int insertPosition{ static_cast<int>(numWithCommas.length() - 3) };
	while (insertPosition > 0) {
		numWithCommas.insert(insertPosition, ",");
		insertPosition -= 3;
	}
	printf("\n%s instructions executed.\n", numWithCommas.c_str());
}

void Emulator::setup_gameboy_components()
{
}

void Emulator::update()
{
	int instructions = 0;
	int cycles = 0;
	while (cycles < kMaxCyclesPerFrame && cpu->is_enabled())
	{
		cycles += gameboy->tick();

		if ((instructions % 200) == 0)
			QApplication::processEvents();
		instructions++;
	}

	main_scene->update();

	main_scene->set_lcd_pixels(gameboy->get_lcd_pixels());
	map_scene->set_map_pixels(ppu_proxy->get_map_pixels());

	QApplication::processEvents();
}

void Emulator::slot_tick()
{
	update();

	main_scene->set_lcd_pixels(gameboy->get_lcd_pixels());
	map_scene->set_map_pixels(ppu_proxy->get_map_pixels());

	QApplication::processEvents();
}

void Emulator::dump_memory()
{
	return;

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

	auto filename{ config.rom_path + ".dump" };

	try {
		FS::write_bytes(filename, gameboy->get_memory(), 0x10000);
	}
	catch (const std::runtime_error& e) {
		QMessageBox::warning(this, "Failed to dump memory", QString::fromStdString(e.what()));
	}

	main_scene->add_message("Dumped RAM to file:");
	main_scene->add_message(filename);
}

void Emulator::keyPressEvent(QKeyEvent* event)
{
	return;

	switch (event->key())
	{
	case Qt::Key_Space:
		config.paused = !config.paused;
		main_scene->add_message(config.paused ? "Paused" : "Unpaused");
		break;
	case Qt::Key_D:
		if (event->modifiers() & Qt::ShiftModifier)
		{
			cpu->debug = !cpu->debug;
			main_scene->add_message(cpu->debug ? "Debug enabled" : "Debug disabled");
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
	return;

	config.map_visible = value;
	map_scene->setVisible(value);

	QSize size;

	if (value)
		size = { (kLcdWidth + kMapWidth) * kScale, kLcdHeight * kScale };
	else
		size = { kLcdWidth * kScale, kLcdHeight * kScale };

	setFixedSize(size);
}

void Emulator::slot_open()
{
	const QString caption{ "Open File" };
	const QString current{ QDir::currentPath() };
	const QString filter{ "ROM (*.gb *.rom *.bin *.dump)" };

	auto path{ QFileDialog::getOpenFileName(this, caption, current, filter).toStdString() };

	if (path.empty()) { return; }

	// gameboy->reset();

	try {
		m_open_rom_callback(FS::read_bytes(path), *this);
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
	return;
	main_scene->set_have_rom(value);
	map_scene->set_have_rom(value);
}
