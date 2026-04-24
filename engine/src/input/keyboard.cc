#include "input/keyboard.hh"

bool Keyboard::operator[](SDL_Scancode key) const { return state_.test(key); }

bool Keyboard::was_pressed(SDL_Scancode key) const { return pressed_.test(key); }

bool Keyboard::was_released(SDL_Scancode key) const { return released_.test(key); }

void Keyboard::flush() { pressed_.reset(); released_.reset(); }

void Keyboard::reset() { pressed_.reset(); released_.reset(); state_.reset(); }

void Keyboard::event(const SDL_Event &event) {
    switch (event.type) {
    case SDL_EVENT_KEY_DOWN:
        state_.set(event.key.scancode, true);
        pressed_.set(event.key.scancode, true);
        break;
    case SDL_EVENT_KEY_UP:
        state_.set(event.key.scancode, false);
        released_.set(event.key.scancode, true);
        break;
    }
}

