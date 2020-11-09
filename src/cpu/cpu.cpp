#include "cpu/cpu.h"

#include <cstdio>

#include "cpu/cpu_base.h"
#include "cpu/cpu_instructions.h"
#include "cpu/inst_data.h"
#include "memory_map.h"
#include "mmu.h"
#include "ppu.h"

Cpu::Cpu(CpuRegisters registers)
    : registers(registers)
{
}

void Cpu::reset(bool skip_bios)
{
    if (skip_bios) {
        pc = 0x0100;
        sp = 0xFFFE;
        af = 0x01B0;
        bc = 0x0013;
        de = 0x00D8;
        hl = 0x014D;
        ime = false;
    }
    else {
        pc = 0;
        sp = 0xFFFF;
        af = 0;
        bc = 0;
        de = 0;
        hl = 0;
        ime = true;
    }
}

bool Cpu::is_enabled() const
{
    return enabled;
}

void Cpu::print_instruction(u16 op, u8 d8, u16 d16)
{
    /* Print info */
    printf("%04X: (%04X) IF:%02X IME:%c  AF:%04X  BC:%04X  DE:%04X  HL:%04X  |%02X|  ",
           pc,
           sp,
           registers.if_,
           ime ? '1' : '.',
           af,
           bc,
           de,
           hl,
           op % 0x100);

    const int fsize {kInstFmtSizes[op]};

    if (fsize == 0) {
        printf(kInstFmts[op]);
    }
    else if (fsize == 8) {
        printf(kInstFmts[op], d8);
    }
    else if (fsize == 16) {
        printf(kInstFmts[op], d16);
    }

    putchar('\n');
}

bool cpu_interrupt(Cpu* cpu, Component* mmu, u8 interrupt_bit)
{
    constexpr static u16 kInterruptVectors[] = {0x40, 0x48, 0x50, 0x58, 0x60};

    if (interrupt_bit > 5) {    // there is no interrupt past bit 5
        fprintf(stderr, "Bad interrupt bit: %02x. Must be one of {0, 1, 2, 3, 4}\n", interrupt_bit);
        return false;
    }

    cpu->ime = false;
    cpu->registers.if_ &= ~(1u << interrupt_bit);
    cpu->sp -= 2;
    mmu->write16(cpu->sp, cpu->pc);

    /* I can't remember why I originally had this. */
    if (0 /* || alt_behaviour */) {
        mmu->write8(--cpu->sp, (cpu->pc & 0xFF00) >> 8);
        u8 new_if = cpu->registers.if_ & cpu->registers.ie;
        if (new_if == 0)
            cpu->pc = 0;
        mmu->write8(--cpu->sp, cpu->pc & 0xFF);
    }

    cpu->pc = kInterruptVectors[interrupt_bit];

    return true;
}

void Cpu::halt_bug()
{
    switch (halt_bug_state) {
        case HaltBug::Triggered:
            halt_bug_state = HaltBug::RepeatNext;
            break;
        case HaltBug::RepeatNext:
            pc--;
            halt_bug_state = HaltBug::None;
            break;
        default:
            break;
    }
}

void Cpu::ei_bug()
{
    switch (ei_bug_state) {
        case EIBug::Triggered:
            ei_bug_state = EIBug::Enable;
            break;
        case EIBug::Enable:
            ime = true;
            ei_bug_state = EIBug::None;
            break;
        default:
            break;
    }
}

bool cpu_process_interrupts(Cpu* cpu, Component* mmu)
{
    bool did_interrupt {false};

    if (cpu->ime && (cpu->registers.ie & cpu->registers.if_)) {
        for (int i = 0; i < 5; i++) {
            if (cpu->registers.ie & (cpu->registers.if_ & (1u << i))) {
                did_interrupt = cpu_interrupt(cpu, mmu, i);
                break;
            }
        }
    }

    return did_interrupt;
}

int Cpu::tick(Component& mmu)
{
    int cycles = 0;

    if (cpu_process_interrupts(this, &mmu)) {
        cycles += 4;
        halted = false;
    }

    u16 op {mmu.read8(pc)};
    u16 d16 {mmu.read16(pc + 1)};
    u8 d8 = d16 & 0xFF;

    if (op == 0xCB)
        op = d8 + 0x100;

    if (debug) {
        print_instruction(op, d8, d16);
    }

    pc += kInstSizes[op];

    if (op > 0xFF)    // CB
        pc++;

    cycles += do_instruction(op, d8, d16, mmu);

    halt_bug();
    ei_bug();

    return cycles;
}

int Cpu::do_instruction(u16 op, u8 d8, u16 d16, Component& mmu)
{
    bool extra_cycles {false};
    kInstructions[op](*this, mmu, d8, d16, extra_cycles);
    return extra_cycles ? kInstCyclesAlt[op] : kInstCycles[op];
}

void Cpu::enable_interrupts()
{
    ei_bug_state = EIBug::Triggered;
}

void Cpu::halt()
{
    if (ime)
        halted = true;
    else
        halt_bug_state = HaltBug::Triggered;
}

void Cpu::set_enabled(bool value)
{
    enabled = value;
}

void Cpu::disable_interrupts()
{
    ime = false;
}
