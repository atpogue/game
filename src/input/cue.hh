#pragma once
#include "core/bitmask.hh"
#include "core/types.hh"
#include <SDL3/SDL_scancode.h>

// It's not clear that this is necessary

struct Keyboard;
struct Mouse;

// Button state
enum class On : u8 {
    Press   = 1 << 0,
    Release = 1 << 1,
    Repeat  = 1 << 2,
};

// Keyboard input modifiers
enum class With : u8 {
    None  = 0,
    Ctrl  = 1 << 0,
    Shift = 1 << 1,
    Alt   = 1 << 2,
};

DEFINE_ENUM_BITWISE_OPERATORS(On)
DEFINE_ENUM_BITWISE_OPERATORS(With)

// Represents keyboard input. Used to associate keys with commands.
struct KeyboardCue {
    SDL_Scancode key;
    With with = With::None;
    On on = On::Press;
    bool operator()(const Keyboard &kb) const;
};

// Represents mouse input. Used to associate buttons with commands.
struct MouseCue {
    Uint8 button;
    With with = With::None;
    On on = On::Press;
    bool operator()(const Mouse &m) const;
    bool operator()(const Mouse &m, const Keyboard &kb) const;
};

