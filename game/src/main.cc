#include "engine/core/random.hh"
#include "engine/core/event.hh"
#include "engine/ecs/pose.hh"
#include "engine/render/camera.hh"
#include "engine/render/textures.hh"
#include "engine/render/window.hh"
#include "engine/world/chunk.hh"
#include "player.hh"
#include "world.hh"
#include <cassert>
#include <glm/common.hpp>
#include <random>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_render.h>

namespace { ///////////////////////////////////////////////////////////////////////////////

    constexpr f32 tile_size = 16.f;

    // This all assumes that none of these variables need the engine or SDL to be initialized first

    auto seed = random_seed();
    SplitMix64 rng(seed);
    std::uniform_int_distribution<u32> dist{0u, chunk_size-1u};

    Chunk chunk;
    Camera camera;

} /////////////////////////////////////////////////////////////////////////////////////////

WindowConfig app_window_config() {
    return { .title = "My Game", .width = 800, .height = 600 };
}

static bool load_terrain() {
    auto atlas = textures::create("assets/kenney-1bitpack.png");
    assert(atlas);
    auto sprite = [atlas](f32 x, f32 y, Color color) {
        return Sprite{atlas, {x * tile_size, y * tile_size, tile_size, tile_size}, color};
    };
    terrains::create("grass-1", sprite(5,0,Color{59,216,114,255}));
    terrains::create("grass-2", sprite(6,0,Color{59,216,114,255}));
    terrains::create("grass-3", sprite(7,0,Color{59,216,114,255}));
    terrains::create("grass-tall", sprite(0,2,Color{59,216,114,255}));
    terrains::create("dirt", sprite(2,0,Color{121,70,75,255}));
    terrains::create("rocks", sprite(2,0,Color{206,197,183,255}));
    return true;
}

static bool load_world(u32 x, u32 y) {
    MyChunkGenerator(seed).generate(x, y, chunk);
    return true;
}

bool app_init() {
    camera.viewport = {800.f/tile_size, 600.f/tile_size};
    camera.zoom = 1.3f;
    return load_terrain()
        && player::create(chunk_size * 0.5f, chunk_size * 0.5f)
        && load_world(dist(rng), dist(rng));
}

void app_step() {
    player::process_input();
}

void app_update(float) {
}

void app_render() {
    auto pose = poses::get(player::get());
    if (pose) camera.position = pose->position;
    chunk.render(camera, tile_size);
}

void app_event(const SDL_Event &event) {
    player::event(event);
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
}

