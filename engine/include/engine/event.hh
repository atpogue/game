#pragma once
#include <SDL3/SDL_events.h>

bool push_event(SDL_Event event);
SDL_Event make_quit_event();

