#ifndef MEMORY_MAP_H
#define MEMORY_MAP_H

#include <map>

constexpr inline uint16_t kHeader          = 0x100;
constexpr inline uint16_t kRomBank0        = 0x150;
constexpr inline uint16_t kRomBanksXX      = 0x4000;
constexpr inline uint16_t kTileRamUnsigned = 0x8000;
constexpr inline uint16_t kTileRamSigned   = 0x9000;
constexpr inline uint16_t kMap0            = 0x9800;
constexpr inline uint16_t kMap1            = 0x9C00;
constexpr inline uint16_t kCartRam         = 0xA000;
constexpr inline uint16_t kWram            = 0xC000;
constexpr inline uint16_t kEchoRam         = 0xE000;
constexpr inline uint16_t kOam             = 0xFE00;
constexpr inline uint16_t kIo              = 0xFF00;
constexpr inline uint16_t kIf              = 0xFF0F;
constexpr inline uint16_t kLcdc            = 0xFF40;
constexpr inline uint16_t kStat            = 0xFF41;
constexpr inline uint16_t kScy             = 0xFF42;
constexpr inline uint16_t kScx             = 0xFF43;
constexpr inline uint16_t kLy              = 0xFF44;
constexpr inline uint16_t kLyc             = 0xFF45;
constexpr inline uint16_t kDmaStartAddr    = 0xFF46;
constexpr inline uint16_t kBgPalette       = 0xFF47;
constexpr inline uint16_t kObj0Palette     = 0xFF48;
constexpr inline uint16_t kObj1Palette     = 0xFF49;
constexpr inline uint16_t kWy              = 0xFF4A;
constexpr inline uint16_t kWx              = 0xFF4B;
constexpr inline uint16_t kZeroPage        = 0xFF80;
constexpr inline uint16_t kIe              = 0xFFFF;

struct romHeader_t
{
	uint8_t logo[0x34]; // 0x0
	char title[0xA]; // 0x34
	char manufacturer[0x4]; // 0x3F
	uint8_t p1; // 0x43
	uint8_t newLicensee[0x2]; // 0x44
	uint8_t p2; // 0x46
	uint8_t cartType; // 0x47
	uint8_t romSize; // 0x48
	uint8_t ramSize; // 0x49
	uint8_t destCode; // 0x4A
	uint8_t oldLicensee; // 0x4B
	uint8_t romVer; // 0x4C
	uint8_t headerChecksum; // 0x4D
	uint16_t globalChecksum; // 0x4E
};

static const std::map<uint8_t, const char *> kCartTypes =
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

#endif // MEMORY_MAP_H
