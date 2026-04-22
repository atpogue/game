#include "input/cue.hh"
#include "input/keyboard.hh"
#include "input/mouse.hh"

static bool keyboard_matches_modifier(const Keyboard &kb, With with) {
    return (!has_flag(with, With::Alt)   || kb[SDL_SCANCODE_LALT]   || kb[SDL_SCANCODE_RALT])
        && (!has_flag(with, With::Shift) || kb[SDL_SCANCODE_LSHIFT] || kb[SDL_SCANCODE_RSHIFT])
        && (!has_flag(with, With::Ctrl)  || kb[SDL_SCANCODE_LCTRL]  || kb[SDL_SCANCODE_RCTRL]);
}

bool KeyboardCue::operator()(const Keyboard &kb) const {
    return keyboard_matches_modifier(kb, with) && (
           has_flag(on, On::Press)   &&  kb.was_pressed(key)
        || has_flag(on, On::Release) &&  kb.was_released(key)
        || has_flag(on, On::Repeat)  && !kb.was_pressed(key) && kb[key]
    );
}

bool MouseCue::operator()(const Mouse &m) const {
    return has_flag(on, On::Press)   &&  m.was_pressed(button)
        || has_flag(on, On::Release) &&  m.was_released(button)
        || has_flag(on, On::Repeat)  && !m.was_pressed(button) && m[button];
}

bool MouseCue::operator()(const Mouse &m, const Keyboard &kb) const {
    return keyboard_matches_modifier(kb, with) && (*this)(m);
}

