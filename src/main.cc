#define SDL_MAIN_USE_CALLBACKS
#include "control/actor.hh"
#include "control/director.hh"
#include "core/random.hh"
#include "ecs/ledger.hh"
#include "ecs/pose.hh"
#include "render/graphics.hh"
#include "render/camera.hh"
#include "ui/menu.hh"
#include "world/chunk.hh"
#include <cassert>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_render.h>

// The length of a fixed time step in seconds
constexpr f32 step_size = 1.f / 30.f;

// Get the number of seconds elapsed since SDL was initialized
f32 time() { return (f32)(SDL_GetTicks()) / 1000.f; }

constexpr f32 window_width=800.f, window_height=400.f;
constexpr f32 tile_size=32.f;

struct AppState {

    f32 lag = 0.f;
    f32 prior = 0.f; // time of start of last frame in seconds

    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
    CommandQueue commands;
    Chunk chunk;
    Camera camera;

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

    state->window = SDL_CreateWindow("Hello World", window_width, window_height, 0);
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
    assert(e && ledger::status(e));
    state->player.entity = e;

    auto pose = poses::set(e, { .position = {chunk_size / 2.f, chunk_size /2.f} });
    assert(pose && poses::get(e));
    assert(ledger::has(e, Flag::Pose));

    actors::create(e, 2);
    assert(ledger::has(e, Flag::Actor));

    auto atlas = graphics::create_texture("assets/tiles.png");
    assert(atlas && graphics::get_texture(atlas));
    if (!terrain::create("grass", {atlas, {0 * tile_size, 0 * tile_size, tile_size, tile_size}}))
        return SDL_APP_FAILURE;
    if (!terrain::create("dirt", {atlas, {1 * tile_size, 0 * tile_size, tile_size, tile_size}}))
        return SDL_APP_FAILURE;

    if (!generate_chunk(random_seed(), 2, 3, state->chunk)) return SDL_APP_FAILURE;

    state->camera.zoom = 1.f;
    state->camera.position = pose->position;
    state->camera.viewport = {window_width/tile_size, window_height/tile_size};

    state->prior = time();
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
    auto *state = static_cast<AppState *>(appstate);
    f32 t = time(), // time of start of current frame in seconds
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
    if (pose) state->camera.position = pose->position;

    // draw the world
    for (auto world_coord : state->camera) {
        u32 x = world_coord.x, y = world_coord.y;
        const Tile *tile = state->chunk.tiles.get(x, y);
        if (!tile) continue;
        const Sprite &sprite = terrain::get(tile->terrain).sprite;
        SDL_Texture *texture = graphics::get_texture(sprite.atlas);
        assert(texture);
        //if (!texture) continue;
        auto coord = state->camera.view_coord_at(world_coord) * tile_size;
        auto scale = tile_size * state->camera.zoom;
        SDL_FRect destination{ coord.x, coord.y, scale, scale };
        SDL_RenderTexture(state->renderer, texture, &sprite.source, &destination);
    }

    // draw the player
    if (pose) {
        auto size = glm::vec2{20.f, 20.f} * state->camera.zoom;
        auto coord = state->camera.view_coord_at(pose->position) * tile_size - size / 2.f;
        SDL_FRect player_square{ coord.x, coord.y, size.x, size.y };
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

