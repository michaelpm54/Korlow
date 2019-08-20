#include <string>
#include <iostream>
#include "args.hxx"
#include "emulator.h"

int main(int argc, char *argv[])
{
	args::ArgumentParser argParser("DMG Gameboy emulator");
	args::ValueFlag<std::string> biosPathArg(argParser, "path-to-bios-file", "", {'B'});
	args::ValueFlag<std::string> romPathArg(argParser, "path-to-rom-file", "", {'R'}, args::Options::Required);
	args::Flag verbosityDebug(argParser, "", "Log general debug information", {'d'});
	args::Flag verbosityRegisters(argParser, "", "Log register contents", {'r'});
	args::Flag verbosityInstructions(argParser, "", "The instructions", {'i'});
	args::Flag verbosityMMU(argParser, "", "Log MMU access", {'m'});
	args::HelpFlag help(argParser, "help", "Show this help", {'h', "help"});

	std::string biosPath;
	std::string romPath;

	try
	{
		argParser.ParseCLI(argc, argv);
	}
	catch (const args::Help)
	{
		std::cout << argParser;
	}
	catch (const args::Error &e)
	{
		std::cerr << e.what() << std::endl << argParser;
		return 1;
	}

	// BIOS defaults to empty, in which case we use the
	// values after the BIOS has theoretically run
	biosPath = biosPathArg ? args::get(biosPathArg) : "";
	romPath = args::get(romPathArg);

	uint8_t verbosityFlags = 0;
	if (args::get(verbosityDebug)) verbosityFlags |= 0x1;
	if (args::get(verbosityRegisters)) verbosityFlags |= 0x2;
	if (args::get(verbosityInstructions)) verbosityFlags |= 0x4;
	if (args::get(verbosityMMU)) verbosityFlags |= 0x8;

	Emulator emu(biosPath, romPath, verbosityFlags);
	return emu.run();
}
