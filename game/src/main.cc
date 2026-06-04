#include "action/move.hh"
#include "component/pose.hh"
#include "director.hh"
#include "engine/core/random.hh"
#include "engine/event.hh"
#include "engine/core/math.hh"
#include "engine/main.hh"
#include "engine/render/camera.hh"
#include "world/terrain.hh"
#include "world/grassland.hh"
#include "registry.hh"
#include <glm/common.hpp>
#include <random>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_render.h>

struct AppState { /////////////////////////////////////////////////////////////////////////

    struct GameData {
        Registry<Components> registry;
        Chunk chunk;
    } game;

    Camera camera;

    u64 seed;
    SplitMix64 rng;

    struct {
        Entity entity;
        PlayerDirector director;
    } player;

    bool load_world() {
        std::uniform_int_distribution<u32> dist{0u, chunk_size-1u};
        u32 x = dist(rng), y = dist(rng);
        GrasslandGenerator(seed).generate(x, y, game.chunk);
        return true;
    }

    bool load_player() {
        player.entity = game.registry.create();
        game.registry.emplace<Pose>(player.entity, Vec2<float>{chunk_size * 0.5f, chunk_size * 0.5f});
        DEBUG_ASSERT(game.registry.has<Pose>(player.entity));
        return true;
    }

    void process_input() {
        for (auto command : player.director.generate()) {
            try_submit_command(game.registry, player.entity, command);
        }
    }

}; ////////////////////////////////////////////////////////////////////////////////////////

AppConfig app_config() {
    return {
        .step_rate = 32,
        .window = { .title = "My Game", .width = 800, .height = 600 },
    };
}

AppState *app_start(int /*argc*/, char *[] /*argv*/) {
    auto seed = random_seed();
    auto state = new AppState{
        .game = {},
        .camera = {
            .position = {0.f, 0.f},
            .viewport = {800.f/tile_size, 600.f/tile_size},
            .zoom = 1.3f,
        },
        .seed = seed,
        .rng = SplitMix64(seed),
        .player = {}
    };

    if (load_terrain()
        && state->load_world()
        && state->load_player())
    {
        return state;
    }
    return nullptr;
}

void app_step(AppState &state) {
    state.process_input();
    if (auto action = state.game.registry.get<MoveAction>(state.player.entity)) {
        switch (act(state.game.registry, state.player.entity, *action)) {
        case ActionResult::Canceled:
        case ActionResult::Complete:
            state.game.registry.erase<MoveAction>(state.player.entity);
            break;
        case ActionResult::Ongoing:
            break;
        }
    }
}

void app_update(AppState &state, nanoseconds) {
    auto pose = state.game.registry.get<Pose>(state.player.entity);
    if (pose) state.camera.position = pose->position;
}

void app_render(AppState &state) {
    state.game.chunk.render(state.camera, tile_size);
}

void app_event(AppState &state, const SDL_Event &event) {
    state.player.director.event(event);
    switch (event.type) {
    case SDL_EVENT_MOUSE_WHEEL:
        state.camera.zoom = glm::clamp(state.camera.zoom + 0.5f * event.wheel.y, 0.7f, 1.9f);
        break;
    case SDL_EVENT_KEY_DOWN:
        switch (event.key.scancode) {
        case SDL_SCANCODE_ESCAPE:
        case SDL_SCANCODE_Q:
            push_event(make_quit_event());
            break;
        case SDL_SCANCODE_SPACE:
            state.load_world();
            break;
        default:
            break;
        }
        break;
    }
}

void app_quit(AppState *state) {
    delete state;
}

