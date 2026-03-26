#include "cue.hh"
#include "keyboard.hh"
#include "mouse.hh"

namespace {

    bool mouse_matches_trigger(const Mouse &m, Uint8 btn, MouseCue::On on) {
        return has_flag(on, MouseCue::On::Press)   &&  m.was_pressed(btn)
            || has_flag(on, MouseCue::On::Release) &&  m.was_released(btn)
            || has_flag(on, MouseCue::On::Repeat)  && !m.was_pressed(btn) && m[btn];
    }

    bool keyboard_matches_trigger(const Keyboard &kb, SDL_Scancode key, KeyboardCue::On on) {
        return has_flag(on, KeyboardCue::On::Press)   &&  kb.was_pressed(key)
            || has_flag(on, KeyboardCue::On::Release) &&  kb.was_released(key)
            || has_flag(on, KeyboardCue::On::Repeat)  && !kb.was_pressed(key) && kb[key];
    }

    bool keyboard_matches_modifier(const Keyboard &kb, KeyboardCue::With with) {
        return (!has_flag(with, KeyboardCue::With::Alt)   || kb[SDL_SCANCODE_LALT]   || kb[SDL_SCANCODE_RALT])
            && (!has_flag(with, KeyboardCue::With::Shift) || kb[SDL_SCANCODE_LSHIFT] || kb[SDL_SCANCODE_RSHIFT])
            && (!has_flag(with, KeyboardCue::With::Ctrl)  || kb[SDL_SCANCODE_LCTRL]  || kb[SDL_SCANCODE_RCTRL]);
    }

}

void translate(const KeyboardCommandMap &map, const Keyboard &kb, CommandQueue &q) {
    for (auto &[cue, cmd] : map) {
        if (keyboard_matches_trigger(kb, cue.key, cue.on)
        &&  keyboard_matches_modifier(kb, cue.with)) {
            q.push_back(cmd);
        }
    }
}

void translate(const MouseCommandMap &map, const Mouse &m, const Keyboard &kb, CommandQueue &q) {
    for (auto &[cue, cmd] : map) {
        if (mouse_matches_trigger(m, cue.button, cue.on)
        &&  keyboard_matches_modifier(kb, cue.with)) {
            q.push_back(cmd);
        }
    }
}

