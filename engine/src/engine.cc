#include <SDL3/SDL_events.h>
#include <SDL3/SDL_timer.h>

// initialize all subsystems
bool engine_init() {
    return true;
}

// progress all subsystems forwards one simulation step
void engine_step() {
}

// progress animations, effects, and user-interface elements
void engine_update(float /*dt*/) {
}

// dispatch events to interested subsystems
void engine_event(const SDL_Event &event) {
    switch (event.type) {
    case SDL_EVENT_WINDOW_FOCUS_LOST:
        SDL_ResetKeyboard();
        break;
    }
}

// clear all subsystems here
void engine_quit() {
}

