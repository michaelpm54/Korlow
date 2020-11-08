#ifndef CPU_H
#define CPU_H

#include <cstdint>

#include "component.h"
#include "emu_types.h"

enum class HaltBug {
    None,
    Triggered,
    RepeatNext,
};

enum class EIBug {
    None,
    Triggered,
    Enable,
};

struct CpuRegisters {
    u8& io;
    u8& if_;
    u8& ie;
};

struct Cpu : Component {
    Cpu(CpuRegisters);

    void reset(bool) override;
    void write8(u16 address, u8 value)
    {
    }
    int tick(Component& mmu);    // Returns # of cycles taken
    int interrupts(u8 mask, Component& mmu);
    void enable_interrupts();
    void disable_interrupts();
    void halt();
    void set_enabled(bool value);
    bool is_enabled() const;
    void print_instruction(u16 op, u8 d8, u16 d16);
    int interrupt_handler(Component& mmu);
    void halt_bug();
    void ei_bug();

    HaltBug halt_bug_state {HaltBug::None};
    EIBug ei_bug_state {EIBug::None};

    bool debug {false};

    CpuRegisters registers;

    u16 pc;
    u16 sp;

    union {
        u16 af;
        struct {
            u8 f;
            u8 a;
        };
    };
    union {
        u16 bc;
        struct {
            u8 c;
            u8 b;
        };
    };
    union {
        u16 de;
        struct {
            u8 e;
            u8 d;
        };
    };
    union {
        u16 hl;
        struct {
            u8 l;
            u8 h;
        };
    };

private:
    bool halted {false};
    bool enabled {true};

    bool ime;
};

#endif    // CPU_H
