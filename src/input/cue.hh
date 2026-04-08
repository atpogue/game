#pragma once
#include "core/bitmask.hh"
#include "control/command.hh"
#include <SDL3/SDL_scancode.h>
#include <map>

struct Keyboard;

// Represents keyboard input. Used to associate keys with commands.
struct KeyboardCue {
    SDL_Scancode key;
    enum class With : unsigned short {
        None  = 0,
        Ctrl  = 1 << 0,
        Shift = 1 << 1,
        Alt   = 1 << 2,
    } with = With::None;
    enum class On : unsigned short {
        Press   = 1 << 0,
        Release = 1 << 1,
        Repeat  = 1 << 2,
    } on = On::Press;
};

DEFINE_ENUM_BITWISE_OPERATORS(KeyboardCue::On)
DEFINE_ENUM_BITWISE_OPERATORS(KeyboardCue::With)

constexpr bool operator<(const KeyboardCue &a, const KeyboardCue &b) {
    return (a.key != b.key) ? a.key < b.key : a.with < b.with;
}

using KeyboardCommandMap = std::map<KeyboardCue, Command>;

void translate(const KeyboardCommandMap &map, const Keyboard &kb, CommandQueue &q);


struct Mouse;

// Represents mouse input. Used to associate buttons with commands.
struct MouseCue {
    using On = KeyboardCue::On;
    using With = KeyboardCue::With;
    Uint8 button;
    With with = With::None;
    On on = On::Press;
};

constexpr bool operator<(const MouseCue &a, const MouseCue &b) {
    return (a.button != b.button) ? a.button < b.button : a.with < b.with;
}

using MouseCommandMap = std::map<MouseCue, Command>;

void translate(const MouseCommandMap &map, const Mouse &m, const Keyboard &kb, CommandQueue &q);

