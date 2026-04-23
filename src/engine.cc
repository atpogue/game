#include "render/textures.hh"
#include "control/actor.hh"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_timer.h>

bool engine_init() {
    // initialize all subsystems here
    return true;
}

void engine_step() {
    // progress all subsystems forwards one simulation step
    actors::step();
}

void engine_update(float dt) {
    // progress animations, effects, and user-interface elements
}

void engine_event(const SDL_Event &event) {
    // dispatch events to interested subsystems
    switch (event.type) {
    case SDL_EVENT_WINDOW_FOCUS_LOST:
        SDL_ResetKeyboard();
        break;
    }
}

void engine_quit() {
    // clear all subsystems here
    textures::quit();
}

