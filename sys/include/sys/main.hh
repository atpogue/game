#pragma once

// These functions need to be defined by an external binary linking against the engine.

union SDL_Event;

struct Runtime;

Runtime* start(int argc, char* argv[]);

void iterate(Runtime& state);

void handle_event(Runtime& state, SDL_Event const& event);

void quit(Runtime* state);

