#ifndef MEMORY_MAP_H
#define MEMORY_MAP_H

constexpr inline uint16_t kHeader = 0x100;
constexpr inline uint16_t kRomBank0 = 0x150;
constexpr inline uint16_t kRomBanksXX = 0x4000;
// PPU
constexpr inline uint16_t kTileRamUnsigned = 0x8000;
constexpr inline uint16_t kTileRamSigned = 0x9000;
constexpr inline uint16_t kMap0 = 0x9800;
constexpr inline uint16_t kMap1 = 0x9C00;
//
constexpr inline uint16_t kCartRam = 0xA000;
constexpr inline uint16_t kWram = 0xC000;
constexpr inline uint16_t kEchoRam = 0xE000;
// PPU
constexpr inline uint16_t kOam = 0xFE00;
//
constexpr inline uint16_t kIo = 0xFF00;
constexpr inline uint16_t kIf = 0xFF0F;
// SND
constexpr inline uint16_t kNr10 = 0xFF10;
constexpr inline uint16_t kNr11 = 0xFF11;
constexpr inline uint16_t kNr12 = 0xFF12;
constexpr inline uint16_t kNr13 = 0xFF13;
constexpr inline uint16_t kNr14 = 0xFF14;
constexpr inline uint16_t kNr21 = 0xFF16;
constexpr inline uint16_t kNr22 = 0xFF17;
constexpr inline uint16_t kNr23 = 0xFF18;
constexpr inline uint16_t kNr24 = 0xFF19;
constexpr inline uint16_t kNr30 = 0xFF1A;
constexpr inline uint16_t kNr31 = 0xFF1B;
constexpr inline uint16_t kNr32 = 0xFF1C;
constexpr inline uint16_t kNr33 = 0xFF1D;
constexpr inline uint16_t kNr34 = 0xFF1E;
constexpr inline uint16_t kNr41 = 0xFF20;
constexpr inline uint16_t kNr42 = 0xFF21;
constexpr inline uint16_t kNr43 = 0xFF22;
constexpr inline uint16_t kNr44 = 0xFF23;
constexpr inline uint16_t kNr50 = 0xFF24;
constexpr inline uint16_t kNr51 = 0xFF25;
constexpr inline uint16_t kNr52 = 0xFF26;
constexpr inline uint16_t kWavePattern = 0xFF30;
// PPU
constexpr inline uint16_t kLcdc = 0xFF40;
constexpr inline uint16_t kStat = 0xFF41;
constexpr inline uint16_t kScy = 0xFF42;
constexpr inline uint16_t kScx = 0xFF43;
constexpr inline uint16_t kLy = 0xFF44;
constexpr inline uint16_t kLyc = 0xFF45;
constexpr inline uint16_t kDmaStartAddr = 0xFF46;
constexpr inline uint16_t kBgPalette = 0xFF47;
constexpr inline uint16_t kObj0Palette = 0xFF48;
constexpr inline uint16_t kObj1Palette = 0xFF49;
constexpr inline uint16_t kWy = 0xFF4A;
constexpr inline uint16_t kWx = 0xFF4B;
//
constexpr inline uint16_t kExitBootRomReg = 0xFF50;
constexpr inline uint16_t kZeroPage = 0xFF80;
constexpr inline uint16_t kIe = 0xFFFF;

#endif // MEMORY_MAP_H
