#pragma once
#define SDL_MAIN_USE_CALLBACKS
#include "engine/render/window.hh"
#include "engine/time.hh"
#include <iostream>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_timer.h>

struct AppConfig {
    hertz step_rate = 0u;
    WindowConfig window;
};

////////////////////////////////////////////////////////////////////////////////
// These need to be defined by an external binary linking against the engine.

struct AppState;

[[nodiscard]] AppConfig app_config();
[[nodiscard]] AppState *app_start(int argc, char *argv[]);
// Advance forwards one fixed-size time step.
void app_step(AppState &state);
void app_update(AppState &state, nanoseconds dt);
void app_render(AppState &state);
void app_event(AppState &state, const SDL_Event &event);
// You are responsible for freeing state.
void app_quit(AppState *state);

////////////////////////////////////////////////////////////////////////////////
// These are internally defined by the engine in different compilation units.

namespace detail {

    [[nodiscard]] bool open_window(WindowConfig config);
    void close_window();

    [[nodiscard]] bool engine_init();
    void engine_step();
    void engine_update(nanoseconds dt);
    void engine_event(const SDL_Event &event);
    void engine_quit();

    [[nodiscard]] SDL_Renderer *get_renderer();

    inline TimeStep time_step(0u);
    inline nanoseconds time_prior = 0u;

}

////////////////////////////////////////////////////////////////////////////////

inline SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv) {
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

inline SDL_AppResult SDL_AppIterate(void *appstate) {
    auto &state = *static_cast<AppState *>(appstate);
    const nanoseconds now = SDL_GetTicksNS();
    nanoseconds dt = now - detail::time_prior;

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

inline SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
    auto &state = *static_cast<AppState *>(appstate);
    detail::engine_event(*event);
    app_event(state, *event);
    switch (event->type) {
    case SDL_EVENT_QUIT:
    case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
        return SDL_APP_SUCCESS;
        break;
    }
    return SDL_APP_CONTINUE;
}

inline void SDL_AppQuit(void *appstate, SDL_AppResult /*result*/) {
    auto state = static_cast<AppState *>(appstate);
    app_quit(state);
    detail::engine_quit();
    detail::close_window();
    SDL_Quit();
}

