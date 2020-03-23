#pragma once
#include <allegro5/keycodes.h>

namespace x39::goingfactory::io
{
    enum class EModifier
    {
        EMPTY = 0,
        SHIFT = 0x00001,
        CTRL = 0x00002,
        ALT = 0x00004,
        LWIN = 0x00008,
        RWIN = 0x00010,
        MENU = 0x00020,
        ALTGR = 0x00040,
        COMMAND = 0x00080,
        SCROLLLOCK = 0x00100,
        NUMLOCK = 0x00200,
        CAPSLOCK = 0x00400,
        INALTSEQ = 0x00800,
        ACCENT1 = 0x01000,
        ACCENT2 = 0x02000,
        ACCENT3 = 0x04000,
        ACCENT4 = 0x08000
    };
}