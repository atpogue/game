#pragma once
#define SDL_MAIN_USE_CALLBACKS
#include "engine/render/window.hh"
#include <iostream>
#include <cmath>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_timer.h>

////////////////////////////////////////////////////////////////////////////////
// These need to be defined by an external binary linking against the engine.

WindowConfig app_window_config();
bool app_init();
void app_step();
void app_update(float dt);
void app_render();
void app_event(const SDL_Event &event);
void app_quit();

////////////////////////////////////////////////////////////////////////////////
// These are internally defined by the engine in different compilation units.

bool open_window(WindowConfig config);
void close_window();
bool engine_init();
bool engine_step();
void engine_update(float dt);
void engine_event(const SDL_Event &event);
void engine_quit();
SDL_Renderer *get_renderer();

////////////////////////////////////////////////////////////////////////////////

inline f32 time_seconds() { return (f32)(SDL_GetTicks()) / 1000.f; }

constexpr f32 step_size = 1.f / 30.f;
inline f32 time_lag = 0.f;
inline f32 time_prior = 0.f;

inline SDL_AppResult SDL_AppInit(void ** /*appstate*/, int /*argc*/, char ** /*argv*/) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "Could not initialize SDL: " << SDL_GetError() << '\n';
        return SDL_APP_FAILURE;
    }

    if (!open_window(app_window_config())) return SDL_APP_FAILURE;
    if (!engine_init()) return SDL_APP_FAILURE;
    if (!app_init()) return SDL_APP_FAILURE;

    time_prior = time_seconds();
    return SDL_APP_CONTINUE;
}

inline SDL_AppResult SDL_AppIterate(void * /*appstate*/) {
    f32 time = time_seconds();
    f32 dt = time - time_prior;
    time_lag += dt;

    while (time_lag >= step_size) {
        // progress the simulation forwards one fixed time step
        engine_step();
        app_step();
        time_lag -= step_size;
    }

    dt = fmin(dt, 0.25f);
    engine_update(dt);
    app_update(dt);

    auto renderer = get_renderer();
    SDL_SetRenderDrawColor(renderer, 73, 49, 62, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    app_render();
    SDL_RenderPresent(renderer);

    time_prior = time;
    return SDL_APP_CONTINUE;
}

inline SDL_AppResult SDL_AppEvent(void * /*appstate*/, SDL_Event *event) {
    engine_event(*event);
    app_event(*event);
    switch (event->type) {
    case SDL_EVENT_QUIT:
    case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
        return SDL_APP_SUCCESS;
        break;
    }
    return SDL_APP_CONTINUE;
}

inline void SDL_AppQuit(void * /*appstate*/, SDL_AppResult /*result*/) {
    app_quit();
    engine_quit();
    close_window();
    SDL_Quit();
}

