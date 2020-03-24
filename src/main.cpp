#include "emulator.h"

#include <iostream>

int main(int argc, char *argv[])
{
	try {
		Emulator(argc, argv).run();
	}
	catch (const std::runtime_error& e)
	{
		std::cerr << e.what() << std::endl;
		return 1;
	}
	return 0;
}
