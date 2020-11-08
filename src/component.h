#ifndef COMPONENT_H
#define COMPONENT_H

#include "emu_types.h"

struct Component {
    Component()
    {
    }

    Component(const Component &) = delete;

    virtual ~Component()
    {
    }

    virtual void reset(bool skip_bios) = 0;

    virtual u8 read8(u16 address)
    {
        return 0xFF;
    }

    virtual u16 read16(u16 address)
    {
        return 0xFFFF;
    }

    virtual void write8(u16 address, u8 value)
    {
    }

    virtual void write16(u16 address, u16 value)
    {
    }
};

#endif    // COMPONENT_H
