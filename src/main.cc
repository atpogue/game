#define SDL_MAIN_USE_CALLBACKS
#include "control/actor.hh"
#include "control/director.hh"
#include "ecs/ledger.hh"
#include "ecs/pose.hh"
#include "render/graphics.hh"
#include "ui/menu.hh"
#include <cassert>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_render.h>

// The length of a fixed time step in seconds
constexpr float step_size = 1.f / 30.f;

// Get the number of seconds elapsed since SDL was initialized
float time() { return float(SDL_GetTicks()) / 1000.f; }

struct AppState {

    float lag = 0.f;
    float prior = 0.f; // time of start of last frame in seconds

    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
    CommandQueue commands;

    struct {
        MainMenu main_menu;
    } ui;

    struct {
        Entity entity;
        PlayerDirector director;
    } player;

};

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "Could not initialize SDL: " << SDL_GetError() << '\n';
        return SDL_APP_FAILURE;
    }

    auto *state = new AppState{};
    *appstate = state;

    state->window = SDL_CreateWindow("Hello World", 800, 400, 0);
    if (!state->window) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not create window: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    state->renderer = SDL_CreateRenderer(state->window, NULL);
    if (!state->renderer) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not create renderer: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    graphics::init(state->renderer);

    auto e = ledger::create();
    assert(e != Entity::null());
    assert(ledger::status(e));
    state->player.entity = e;

    poses::set(e, { .position = {400.f, 200.f} });
    assert(poses::get(e));
    assert(ledger::has(e, Flag::Pose));

    actors::create(e, 2);
    assert(ledger::has(e, Flag::Actor));

    state->prior = time();
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
    auto *state = static_cast<AppState *>(appstate);
    float t = time(), // time of start of current frame in seconds
          dt = t - state->prior; // time that elapsed since the start of last frame

    /// STEP ////////////////////////////////////////////////////////////
    for (state->lag += dt; state->lag >= step_size; state->lag -= step_size) {
        // progress the simulation forwards one fixed time step
        state->player.director.generate(state->commands);
        actors::act(state->player.entity, state->commands);
        state->commands.clear();
        actors::step();
    }

    /// UPDATE //////////////////////////////////////////////////////////
    dt = fmin(dt, 0.25f);
    state->ui.main_menu.update(dt);

    /// RENDER //////////////////////////////////////////////////////////
    SDL_SetRenderDrawColor(state->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(state->renderer);

    state->ui.main_menu.render(state->renderer);

    auto pose = poses::get(state->player.entity);
    if (pose) {
        SDL_FRect player_square{pose->position.x - 10.f, pose->position.y - 10.f, 20.f, 20.f};
        SDL_SetRenderDrawColor(state->renderer, 0, 0, 0, 255);
        SDL_RenderFillRect(state->renderer, &player_square);
    }

    SDL_RenderPresent(state->renderer);

    state->prior = t;
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
    if (!appstate) return SDL_APP_FAILURE;
    if (!event) return SDL_APP_CONTINUE;
    auto *state = static_cast<AppState *>(appstate);

    // process all window events and other high priority events 
    switch (event->type) {
    case SDL_EVENT_QUIT:
    case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
        return SDL_APP_SUCCESS;
        break;
    case SDL_EVENT_WINDOW_FOCUS_LOST:
        SDL_ResetKeyboard();
        break;
    }

    // dispatch events to interested systems
    state->ui.main_menu.event(*event);
    state->player.director.event(*event);
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
    auto *state = static_cast<AppState *>(appstate);
    if (state) {
        graphics::quit(); // destroy all the textures
        SDL_DestroyRenderer(state->renderer);
        SDL_DestroyWindow(state->window);
        delete state;
    }
    SDL_Quit();
}

