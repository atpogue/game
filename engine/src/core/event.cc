#include "core/event.hh"
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_log.h>

bool push_event(SDL_Event event) {
    if (!SDL_PushEvent(&event)) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not push event: %s\n", SDL_GetError());
        return false;
    }
    return true;
}

SDL_Event make_quit_event() {
    SDL_Event event;
    event.type = SDL_EVENT_QUIT;
    event.quit.timestamp = SDL_GetTicksNS();
    return event;
}

