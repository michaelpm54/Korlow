#ifndef CONSTANTS_H
#define CONSTANTS_H

constexpr int kScale { 4 };

constexpr int kLcdWidth { 160 };
constexpr int kLcdHeight { 144 };

constexpr int kMapWidth { 32 };
constexpr int kMapHeight { 64 };

constexpr const char *kQuadVertexShader { "assets/shaders/ssquad.vs" };
constexpr const char *kQuadFragmentShader { "assets/shaders/ssquad.fs" };

constexpr const char *kTextVertexShader { "assets/shaders/font.vs" };
constexpr const char *kTextFragmentShader { "assets/shaders/font.fs" };

constexpr float kClearColor[3] { 0.25f, 0.04f, 0.18f };

constexpr int kCpuFreq { 4'194'304 };
constexpr int kMaxCyclesPerFrame { kCpuFreq / 60 };

#endif // CONSTANTS_H
