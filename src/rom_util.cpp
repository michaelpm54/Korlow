#include "rom_util.h"

#include <cstdio>
#include <map>

void printRomInfo(const std::vector<std::uint8_t> &rom)
{
    struct RomHeader {
        uint8_t logo[0x34];          // 0x00
        char title[0xA];             // 0x34
        char manufacturer[0x4];      // 0x3F
        uint8_t p1;                  // 0x43
        uint8_t newLicensee[0x2];    // 0x44
        uint8_t p2;                  // 0x46
        uint8_t cartType;            // 0x47
        uint8_t romSize;             // 0x48
        uint8_t ramSize;             // 0x49
        uint8_t destCode;            // 0x4A
        uint8_t oldLicensee;         // 0x4B
        uint8_t romVer;              // 0x4C
        uint8_t headerChecksum;      // 0x4D
        uint16_t globalChecksum;     // 0x4E
    };

    static std::map<uint8_t, const char *> kCartTypes =
        {
            {0x00, "ROM ONLY"},
            {0x01, "MBC1"},
            {0x02, "MBC1+RAM"},
            {0x03, "MBC1+RAM+BATTERY"},
            {0x05, "MBC2"},
            {0x06, "MBC2+BATTERY"},
            {0x08, "ROM+RAM"},
            {0x09, "ROM+RAM+BATTERY"},
            {0x0B, "MMM01"},
            {0x0C, "MMM01+RAM"},
            {0x0D, "MMM01+RAM+BATTERY"},
            {0x0F, "MBC3+TIMER+BATTERY"},
            {0x10, "MBC3+TIMER+RAM+BATTERY"},
            {0x11, "MBC3"},
            {0x12, "MBC3+RAM"},
            {0x13, "MBC3+RAM+BATTERY"},
            {0x15, "MBC4"},
            {0x16, "MBC4+RAM"},
            {0x17, "MBC4+RAM+BATTERY"},
            {0x19, "MBC5"},
            {0x1A, "MBC5+RAM"},
            {0x1B, "MBC5+RAM+BATTERY"},
            {0x1C, "MBC5+RUMBLE"},
            {0x1D, "MBC5+RUMBLE+RAM"},
            {0x1E, "MBC5+RUMBLE+RAM+BATTERY"},
            {0xFC, "POCKET CAMERA"},
            {0xFD, "BANDAI TAMA5"},
            {0xFE, "HuC3"},
            {0xFF, "HuC1+RAM+BATTERY"},
        };

    RomHeader romHeader;
    std::memcpy(&romHeader, &rom[0x100], sizeof(RomHeader));

    printf("Title: %s\n", romHeader.title);
    printf("ROM Size: %d\n", (2 << 14) << romHeader.romSize);
    printf("Cart type: %s\n", kCartTypes.at(romHeader.cartType));
}

std::vector<std::uint8_t> ghostRom()
{
    std::vector<std::uint8_t> rom(0x4E);

    static constexpr uint8_t logo[] =
        {
            0xCE,
            0xED,
            0x66,
            0x66,
            0xCC,
            0x0D,
            0x00,
            0x0B,
            0x03,
            0x73,
            0x00,
            0x83,
            0x00,
            0x0C,
            0x00,
            0x0D,
            0x00,
            0x08,
            0x11,
            0x1F,
            0x88,
            0x89,
            0x00,
            0x0E,
            0xDC,
            0xCC,
            0x6E,
            0xE6,
            0xDD,
            0xDD,
            0xD9,
            0x99,
            0xBB,
            0xBB,
            0x67,
            0x63,
            0x6E,
            0x0E,
            0xEC,
            0xCC,
            0xDD,
            0xDC,
            0x99,
            0x9F,
            0xBB,
            0xB9,
            0x33,
            0x3E};
    std::memcpy(&rom[0x4], logo, sizeof(logo));

    // a = 0x19 at this point
    // a += mem[0x14D] needs to be equal to 0
    // 0x19 + 0xE7 = 0x100 or 0x00 as a byte
    rom[0x4D] = 0xE7;    // checksum

    // infinite loop
    rom[0x00] = 0x18;    // jr
    rom[0x01] = 0xFE;    // -2

    return rom;
}
