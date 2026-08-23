#define SDL_MAIN_USE_CALLBACKS

#include "sys/main.hh"
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_timer.h>
#include <print>

SDL_AppResult SDL_AppInit(void** state, int argc, char** argv)
{
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    std::println("Failed to initialize SDL: {}", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  *state = start(argc, argv);
  if (!*state) {
    std::println("Runtime state not created. Quiting...");
    return SDL_APP_FAILURE;
  }

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* state)
{
  auto runtime = static_cast<Runtime*>(state);
  iterate(*runtime);
  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* state, SDL_Event* event)
{
  auto runtime = static_cast<Runtime*>(state);
  handle_event(*runtime, *event);
  switch (event->type) {
  case SDL_EVENT_QUIT:
    return SDL_APP_SUCCESS;
  case SDL_EVENT_WINDOW_FOCUS_LOST:
    SDL_ResetKeyboard();
    break;
  default:
    break;
  }
  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* state, SDL_AppResult /*result*/)
{
  auto runtime = static_cast<Runtime*>(state);
  quit(runtime);
  SDL_Quit();
}

