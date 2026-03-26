#pragma once
#include <bitset>
#include <SDL3/SDL_events.h>

struct Mouse {

    static const unsigned short BUTTON_COUNT = 16;

    // is the button currently active?
    bool operator[](Uint8 btn) const;

    // did the button see a [SDL_MOUSE_BUTTON_DOWN] event since the last flush?
    bool was_pressed(Uint8 btn) const;

    // did the button see a [SDL_MOUSE_BUTTON_UP] event since the last flush?
    bool was_released(Uint8 btn) const;

    float x()  const;
    float y()  const;
    float dx() const;
    float dy() const;
    float wheel_dx() const;
    float wheel_dy() const;

    // forget all events that have occured, use after accounting for events
    void flush();

    void reset();

    void event(const SDL_Event &event);

private:

    std::bitset<BUTTON_COUNT> state_, pressed_, released_;

    float x_, y_, dx_, dy_, wdx_, wdy_;

};

