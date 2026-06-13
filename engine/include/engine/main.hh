#pragma once

/// Only one source file per executable target should include this file.

#define SDL_MAIN_USE_CALLBACKS
#include "engine/render/window.hh"
#include "engine/time.hh"

#include <iostream>

#include <SDL3/SDL_main.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_timer.h>

struct AppConfig
{
  hertz        step_rate = 0u;
  WindowConfig window;
};

////////////////////////////////////////////////////////////////////////////////
// These need to be defined by an external binary linking against the engine.

/// An opaque type defined by the application; the engine only holds and passes
/// a pointer to it.
struct AppState;

/// Called before `app_start`. Configure application details (window size, name,
/// and step rate) here.
[[nodiscard]] AppConfig app_config();

/// Receives command line arguments. Create `AppState` here. Returning null
/// indicates application failure and `app_quit` will follow.
[[nodiscard]] AppState* app_start(int argc, char* argv[]);

/// Called 0 or more times per frame, once per elapsed timestep. Update
/// simulation systems and anything that needs determinism here. Time that can't
/// be processed in this frame will be deferred to the next frame, accumulating
/// lag.
void app_step(AppState& state);

/// Called once per frame. Advances by the time elapsed since the last frame,
/// capped by 4 hertz. Update user-interface, graphics, animations, and anything
/// that doesn't need determinism here.
void app_update(AppState& state, nanoseconds dt);

/// Called once per frame after `app_update` and `app_step`. Render graphics
/// here.
void app_render(AppState& state);

void app_event(AppState& state, const SDL_Event& event);

/// Called before ending the program. You are responsible for freeing
/// `AppState`.
void app_quit(AppState* state);

////////////////////////////////////////////////////////////////////////////////
// These are internally defined by the engine in different compilation units.

namespace detail {

  [[nodiscard]] bool open_window(WindowConfig config);
  void               close_window();

  [[nodiscard]] bool engine_init();
  void               engine_step();
  void               engine_update(nanoseconds dt);
  void               engine_event(const SDL_Event& event);
  void               engine_quit();

  [[nodiscard]] SDL_Renderer* get_renderer();

  inline TimeStep    time_step(0u);
  inline nanoseconds time_prior = 0u;

} // namespace detail

////////////////////////////////////////////////////////////////////////////////

inline SDL_AppResult SDL_AppInit(void** appstate, int argc, char** argv)
{
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    std::cerr << "Could not initialize SDL: " << SDL_GetError() << '\n';
    return SDL_APP_FAILURE;
  }

  const AppConfig config = app_config();
  detail::time_step.reset(from_hertz(config.step_rate));
  if (!detail::open_window(config.window)) return SDL_APP_FAILURE;
  if (!detail::engine_init()) return SDL_APP_FAILURE;
  *appstate = app_start(argc, argv);
  if (!*appstate) return SDL_APP_FAILURE;

  detail::time_prior = SDL_GetTicksNS();
  return SDL_APP_CONTINUE;
}

inline SDL_AppResult SDL_AppIterate(void* appstate)
{
  auto&             state = *static_cast<AppState*>(appstate);
  const nanoseconds now   = SDL_GetTicksNS();
  nanoseconds       dt    = now - detail::time_prior;

  for (auto steps = detail::time_step.advance(dt); steps > 0u; steps--) {
    // progress the simulation forwards one fixed-size time step
    detail::engine_step();
    app_step(state);
  }

  dt = dt > from_hertz(4u) ? from_hertz(4u) : dt;
  detail::engine_update(dt);
  app_update(state, dt);

  auto renderer = detail::get_renderer();
  SDL_SetRenderDrawColor(renderer, 73, 49, 62, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(renderer);
  app_render(state);
  SDL_RenderPresent(renderer);

  detail::time_prior = now;
  return SDL_APP_CONTINUE;
}

inline SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
  auto& state = *static_cast<AppState*>(appstate);
  detail::engine_event(*event);
  app_event(state, *event);
  switch (event->type) {
  case SDL_EVENT_QUIT:
  case SDL_EVENT_WINDOW_CLOSE_REQUESTED: return SDL_APP_SUCCESS; break;
  }
  return SDL_APP_CONTINUE;
}

inline void SDL_AppQuit(void* appstate, SDL_AppResult /*result*/)
{
  auto state = static_cast<AppState*>(appstate);
  app_quit(state);
  detail::engine_quit();
  detail::close_window();
  SDL_Quit();
}

