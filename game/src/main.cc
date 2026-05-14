#include "director.hh"
#include "engine/component/pose.hh"
#include "engine/component/actor.hh"
#include "engine/core/random.hh"
#include "engine/event.hh"
#include "engine/registry.hh"
#include "engine/render/camera.hh"
#include "engine/render/textures.hh"
#include "engine/render/window.hh"
#include "engine/world/terrain.hh"
#include "procgen/world.hh"
#include <cassert>
#include <glm/common.hpp>
#include <random>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_render.h>

namespace { ///////////////////////////////////////////////////////////////////////////////

    // This all assumes that none of these variables need the engine or SDL to be initialized first

    constexpr f32 tile_size = 16.f;

    struct {
        Entity entity;
        PlayerDirector director;
    } player;

    auto seed = random_seed();
    SplitMix64 rng(seed);
    std::uniform_int_distribution<u32> dist{0u, chunk_size-1u};

    Registry registry;

    Chunk chunk;
    Camera camera;

} /////////////////////////////////////////////////////////////////////////////////////////

WindowConfig app_window_config() {
    return { .title = "My Game", .width = 800, .height = 600 };
}

static bool load_terrain() {
    auto atlas = create_texture("assets/kenney-1bitpack.png");
    assert(atlas);
    auto sprite = [atlas](f32 x, f32 y, Color color) {
        return Sprite{atlas, {x * tile_size, y * tile_size, tile_size, tile_size}, color};
    };
    create_terrain("grass-1", sprite(5,0,Color{59,216,114,255}));
    create_terrain("grass-2", sprite(6,0,Color{59,216,114,255}));
    create_terrain("grass-3", sprite(7,0,Color{59,216,114,255}));
    create_terrain("grass-tall", sprite(0,2,Color{59,216,114,255}));
    create_terrain("dirt", sprite(2,0,Color{121,70,75,255}));
    create_terrain("rocks", sprite(2,0,Color{206,197,183,255}));
    return true;
}

static void load_world(u32 x, u32 y) {
    WorldGenerator(seed).generate(x, y, chunk);
}

void create_player(f32 x, f32 y) {
    player.entity = registry.create();
    registry.emplace<Actor>(player.entity, 2);
    registry.emplace<Pose>(player.entity, glm::vec2{x, y});
}

void process_input() {
    auto actor = registry.get<Actor>(player.entity);
    if (!actor) return;
    for (auto command : player.director.generate()) {
        // TODO: check for room before allocating an action
        actor->try_submit(make_action(registry, player.entity, command));
    }
}

bool app_init() {
    camera.viewport = {800.f/tile_size, 600.f/tile_size};
    camera.zoom = 1.3f;
    create_player(chunk_size * 0.5f, chunk_size * 0.5f);
    assert(registry.has<Actor>(player.entity));
    assert(registry.has<Pose>(player.entity));
    if (!load_terrain()) return false;
    load_world(dist(rng), dist(rng));
    return true;
}

void app_step() {
    // TODO: ability to query all entities matching a component signature
    process_input();
    auto actor = registry.get<Actor>(player.entity);
    if (actor) actor->step(registry, player.entity);
}

void app_update(float) {
    auto pose = registry.get<Pose>(player.entity);
    if (pose) camera.position = pose->position;
}

void app_render() {
    chunk.render(camera, tile_size);
}

void app_event(const SDL_Event &event) {
    player.director.event(event);
    switch (event.type) {
    case SDL_EVENT_MOUSE_WHEEL:
        camera.zoom = glm::clamp(camera.zoom + 0.5f * event.wheel.y, 0.7f, 1.9f);
        break;
    case SDL_EVENT_KEY_DOWN:
        switch (event.key.scancode) {
        case SDL_SCANCODE_ESCAPE:
        case SDL_SCANCODE_Q:
            push_event(make_quit_event());
            break;
        case SDL_SCANCODE_SPACE:
            load_world(dist(rng), dist(rng));
            break;
        default:
            break;
        }
        break;
    }
}

void app_quit() {
    registry.clear();
}

