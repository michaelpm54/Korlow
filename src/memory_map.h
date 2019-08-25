#ifndef MEMORY_MAP_H
#define MEMORY_MAP_H

constexpr inline uint16_t kHeader          = 0x100;
constexpr inline uint16_t kRomBank0        = 0x150;
constexpr inline uint16_t kRomBanksXX      = 0x4000;
constexpr inline uint16_t kTileRamUnsigned = 0x8000;
constexpr inline uint16_t kTileRamSigned   = 0x8800;
constexpr inline uint16_t kBgMap0          = 0x9800;
constexpr inline uint16_t kBgMap1          = 0x9C00;
constexpr inline uint16_t kCartRam         = 0xA000;
constexpr inline uint16_t kWram            = 0xC000;
constexpr inline uint16_t kEchoRam         = 0xE000;
constexpr inline uint16_t kOam             = 0xFE00;
constexpr inline uint16_t kIo              = 0xFF00;
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
constexpr inline uint16_t kIeFlag          = 0xFFFF;

struct Header
{
	uint8_t p0[0x34];
	char title[0xA];
	char manufacturer[0x4];
	uint8_t p1;
	uint8_t newLicensee[0x2];
	uint8_t p2;
	uint8_t cartType;
	uint8_t romSize;
	uint8_t ramSize;
	uint8_t destCode;
	uint8_t oldLicensee;
	uint8_t headerChecksum;
	uint16_t globalChecksum;
};

#endif // MEMORY_MAP_H
