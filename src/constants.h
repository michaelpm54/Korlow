#ifndef CONSTANTS_H
#define CONSTANTS_H

constexpr int kLcdWidth {160};
constexpr int kLcdHeight {144};

constexpr int kMapWidth {32};
constexpr int kMapHeight {32};

constexpr int kCpuFreq {4'194'304};
constexpr int kMaxCyclesPerFrame {kCpuFreq / 60};

#endif    // CONSTANTS_H
