#pragma once
#include <bitset>
#include <SDL3/SDL_events.h>

class Keyboard {
public:

    // is the key currently active?
    bool operator[](SDL_Scancode key) const;

    // did the key see a [SDL_EVENT_KEY_DOWN] event since the last flush?
    bool was_pressed(SDL_Scancode key) const;

    // did the key see a [SDL_EVENT_KEY_UP] event since the last flush?
    bool was_released(SDL_Scancode key) const;

    // forget all events that have occured, use after accounting for events
    void flush();

    void reset();

    void event(const SDL_Event &event);

private:

    std::bitset<SDL_SCANCODE_COUNT> state_, pressed_, released_;

};

