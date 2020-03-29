#include "emulator.h"

#include <iostream>

#include <QApplication>

int main(int argc, char *argv[])
{
	try
	{
		QApplication app(argc, argv);

		auto emu = Emulator();

		app.exec();
	}
	catch (const std::runtime_error& e)
	{
		std::cerr << e.what() << std::endl;
		return 1;
	}
	return 0;
}
