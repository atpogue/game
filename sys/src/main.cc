#define SDL_MAIN_USE_CALLBACKS

#include "sys/main.hh"
#include "sys/time.hh"
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_timer.h>
#include <print>

namespace {
  TimeStep    timestep(0u);
  Nanoseconds time_prior = 0u;
} // namespace

SDL_AppResult SDL_AppInit(void** appstate, int argc, char** argv)
{
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    std::println("Failed to initialize SDL: {}", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  AppConfig const config = configure(argc, argv);

  *appstate = start();
  if (!*appstate) {
    std::println("Runtime state not created. Quiting...");
    return SDL_APP_FAILURE;
  }

  timestep.resize(from_hertz(config.step_rate));
  time_prior = SDL_GetTicksNS();
  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate)
{
  auto              state = static_cast<AppState*>(appstate);
  Nanoseconds const now   = SDL_GetTicksNS();
  Nanoseconds       dt    = now - time_prior;

  for (auto steps = timestep.advance(dt); steps > 0u; steps--) { step(*state); }

  update(*state, to_seconds(dt));

  render(*state, timestep.alpha());

  time_prior = now;
  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
  auto state = static_cast<AppState*>(appstate);
  handle_event(*state, *event);
  switch (event->type) {
  case SDL_EVENT_QUIT:
  case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
    return SDL_APP_SUCCESS;
  case SDL_EVENT_WINDOW_FOCUS_LOST:
    SDL_ResetKeyboard();
    break;
  default:
    break;
  }
  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult /*result*/)
{
  auto state = static_cast<AppState*>(appstate);
  quit(state);
  SDL_Quit();
}

