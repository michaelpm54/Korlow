#ifndef MEMORY_MAP_H
#define MEMORY_MAP_H

constexpr inline u16 kHeader = 0x100;
constexpr inline u16 kRomBank0 = 0x150;
constexpr inline u16 kRomBanksXX = 0x4000;
// PPU
constexpr inline u16 kTileRamUnsigned = 0x8000;
constexpr inline u16 kTileRamSigned = 0x9000;
constexpr inline u16 kMap0 = 0x9800;
constexpr inline u16 kMap1 = 0x9C00;
//
constexpr inline u16 kCartRam = 0xA000;
constexpr inline u16 kWram = 0xC000;
constexpr inline u16 kEchoRam = 0xE000;
// PPU
constexpr inline u16 kOam = 0xFE00;
//
constexpr inline u16 kIo = 0xFF00;
constexpr inline u16 kDiv = 0xFF04;
constexpr inline u16 kTima = 0xFF05;
constexpr inline u16 kTma = 0xFF06;
constexpr inline u16 kTac = 0xFF07;
constexpr inline u16 kIf = 0xFF0F;
// SND
constexpr inline u16 kNr10 = 0xFF10;
constexpr inline u16 kNr11 = 0xFF11;
constexpr inline u16 kNr12 = 0xFF12;
constexpr inline u16 kNr13 = 0xFF13;
constexpr inline u16 kNr14 = 0xFF14;
constexpr inline u16 kNr21 = 0xFF16;
constexpr inline u16 kNr22 = 0xFF17;
constexpr inline u16 kNr23 = 0xFF18;
constexpr inline u16 kNr24 = 0xFF19;
constexpr inline u16 kNr30 = 0xFF1A;
constexpr inline u16 kNr31 = 0xFF1B;
constexpr inline u16 kNr32 = 0xFF1C;
constexpr inline u16 kNr33 = 0xFF1D;
constexpr inline u16 kNr34 = 0xFF1E;
constexpr inline u16 kNr41 = 0xFF20;
constexpr inline u16 kNr42 = 0xFF21;
constexpr inline u16 kNr43 = 0xFF22;
constexpr inline u16 kNr44 = 0xFF23;
constexpr inline u16 kNr50 = 0xFF24;
constexpr inline u16 kNr51 = 0xFF25;
constexpr inline u16 kNr52 = 0xFF26;
constexpr inline u16 kWavePattern = 0xFF30;
// PPU
constexpr inline u16 kLcdc = 0xFF40;
constexpr inline u16 kStat = 0xFF41;
constexpr inline u16 kScy = 0xFF42;
constexpr inline u16 kScx = 0xFF43;
constexpr inline u16 kLy = 0xFF44;
constexpr inline u16 kLyc = 0xFF45;
constexpr inline u16 kDmaStartAddr = 0xFF46;
constexpr inline u16 kBgPalette = 0xFF47;
constexpr inline u16 kObj0Palette = 0xFF48;
constexpr inline u16 kObj1Palette = 0xFF49;
constexpr inline u16 kWy = 0xFF4A;
constexpr inline u16 kWx = 0xFF4B;
//
constexpr inline u16 kExitBootRomReg = 0xFF50;
constexpr inline u16 kZeroPage = 0xFF80;
constexpr inline u16 kIe = 0xFFFF;

#endif    // MEMORY_MAP_H
