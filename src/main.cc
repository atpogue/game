#define SDL_MAIN_USE_CALLBACKS
#include "core/random.hh"
#include "core/render.hh"
#include <iostream>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_timer.h>

bool app_init(SDL_Window *window, SDL_Renderer *renderer);
void app_update(float dt);
void app_event(const SDL_Event &event);
void app_render(SDL_Renderer *renderer);
void app_quit();

f32 time_seconds() { return (f32)(SDL_GetTicks()) / 1000.f; }

constexpr f32 window_width=800.f, window_height=400.f;

struct Application {

    f32 time_lag = 0.f;
    f32 time_prior = 0.f;

    Window window;
    Renderer renderer;

};

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "Could not initialize SDL: " << SDL_GetError() << '\n';
        return SDL_APP_FAILURE;
    }

    auto *state = new Application{};
    *appstate = state;

    state->window.reset(SDL_CreateWindow("Hello World", window_width, window_height, 0));
    if (!state->window) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not create window: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    state->renderer.reset(SDL_CreateRenderer(state->window.get(), NULL));
    if (!state->renderer) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not create renderer: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!app_init(state->window.get(), state->renderer.get())) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to initialize!");
        return SDL_APP_FAILURE;
    };

    state->time_prior = time_seconds();
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
    auto *state = static_cast<Application *>(appstate);
    auto time = time_seconds(); // time of start of current frame in seconds
    app_update(time - state->time_prior);
    SDL_SetRenderDrawColor(state->renderer.get(), 73, 49, 62, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(state->renderer.get());
    app_render(state->renderer.get());
    SDL_RenderPresent(state->renderer.get());
    state->time_prior = time;
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
    auto *state = static_cast<Application *>(appstate);
    app_event(*event);
    switch (event->type) {
    case SDL_EVENT_QUIT:
    case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
        return SDL_APP_SUCCESS;
        break;
    case SDL_EVENT_WINDOW_FOCUS_LOST:
        SDL_ResetKeyboard();
        break;
    }
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
    auto *state = static_cast<Application *>(appstate);
    app_quit();
    delete state;
    SDL_Quit();
}

