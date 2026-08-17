#define SDL_MAIN_USE_CALLBACKS

#include "engine/app.hh"
#include "engine/time.hh"
#include "render/internal.hh"
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_timer.h>
#include <print>

bool open_window(WindowConfig config);
void close_window();
void destroy_all_textures();

namespace {
  TimeStep    time_step(0u);
  Nanoseconds time_prior = 0u;
} // namespace

SDL_AppResult SDL_AppInit(void** appstate, int argc, char** argv)
{
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    std::println("Failed to initialize SDL: {}", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  AppConfig const config = configure(argc, argv);

  if (!open_window(config.window)) {
    std::println("Failed to create window: {}", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  *appstate = start();
  if (!*appstate) return SDL_APP_FAILURE;

  time_step.reset(from_hertz(config.step_rate));
  time_prior = SDL_GetTicksNS();
  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate)
{
  auto              state = static_cast<AppState*>(appstate);
  Nanoseconds const now   = SDL_GetTicksNS();
  Nanoseconds       dt    = now - time_prior;

  for (auto steps = time_step.advance(dt); steps > 0u; steps--) { step(*state); }

  dt = dt > from_hertz(4u) ? from_hertz(4u) : dt;
  update(*state, dt);

  render(*state); // TODO: alpha
  SDL_RenderPresent(detail::get_renderer());

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
  destroy_all_textures();
  close_window();
  SDL_Quit();
}

