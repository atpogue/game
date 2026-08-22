#pragma once

// These functions need to be defined by an external binary linking against the engine.

#include "sys/time.hh"

union SDL_Event;

struct AppConfig
{
  Hertz step_rate = 32u;
};

// An opaque type defined by the application; the engine only holds and passes a pointer to it.
struct AppState;

// Called before `app_start`. Receives command line arguments.
// Configure application details (window size, name, and step rate) here.
[[nodiscard]] AppConfig configure(int argc, char* argv[]);

// Create `AppState` here. Returning null indicates application failure and `app_quit` will follow.
[[nodiscard]] AppState* start();

// Called 0 or more times per frame, once per elapsed timestep. Update simulation systems and
// anything that needs determinism here. Time that can't be processed in this frame will be deferred
// to the next frame, accumulating lag.
void step(AppState& state);

// Called once per frame. Advances by the time elapsed since the last frame, capped by 4 hertz.
// Update user-interface, graphics, animations, and anything that doesn't need determinism here.
void update(AppState& state, f32 delta);

// Called once per frame after `app_update` and `app_step`.
void render(AppState& state, f32 alpha);

void handle_event(AppState& state, SDL_Event const& event);

// Called before ending the program. You are responsible for freeing `AppState`.
void quit(AppState* state);

