#include "caves.hh"
#include "engine/core/random.hh"
#include "engine/core/event.hh"
#include "engine/input/keyboard.hh"
#include "engine/render/camera.hh"
#include "engine/render/color.hh"
#include "engine/render/draw.hh"
#include "engine/render/window.hh"
#include <cassert>
#include <glm/common.hpp>
#include <random>
#include <SDL3/SDL.h>

namespace { ///////////////////////////////////////////////////////////////////////////////

    constexpr f32 tile_size = 16.f;
    constexpr u32 cave_width = 64u, cave_height = 64u;
    u32 iterations = 0u;

    Camera camera;
    Keyboard keyboard;
    Grid2<u32> *cave = nullptr;

} /////////////////////////////////////////////////////////////////////////////////////////

WindowConfig app_window_config() {
    return { .title = "Cave Generation Demo", .width = 800, .height = 600 };
}

static void new_cave() {
    // uniformly random fill
    Xoshiro256ss rng{random_seed()};
    std::bernoulli_distribution coin{0.45f};
    for (auto &t : *cave) t = coin(rng) ? 1u : 0u;
    iterations = 16u;
}

bool app_init() {
    assert(!cave);
    cave = new Grid2<u32>(cave_width, cave_height);
    new_cave();
    camera.viewport = {800.f/tile_size, 600.f/tile_size};
    camera.zoom = 1.3f;
    return true;
}

void app_step() {
    if (keyboard[SDL_SCANCODE_W]) camera.position.y -= 1.f;
    if (keyboard[SDL_SCANCODE_S]) camera.position.y += 1.f;
    if (keyboard[SDL_SCANCODE_A]) camera.position.x -= 1.f;
    if (keyboard[SDL_SCANCODE_D]) camera.position.x += 1.f;
    keyboard.flush();
    if (iterations) {
        generate_cave(*cave, 1u, 0u, 5u, 6u, 1u, 1u);
        --iterations;
    }
}

void app_update(float) {
}

void app_render() {
    for (auto coord : camera) {
        u32 x = coord.x, y = coord.y;
        auto tile = cave->get(x, y);
        if (!tile) continue;
        auto pixel = camera.view_coord_at({x,y}) * tile_size;
        SDL_Color color = (*tile == 1u) ? hex_color(0x000000) : hex_color(0xFFFFFF);
        draw_rectangle({pixel.x, pixel.y, tile_size * camera.zoom, tile_size * camera.zoom}, color);
    }
}

void app_event(const SDL_Event &event) {
    keyboard.event(event);
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
        case SDL_SCANCODE_RETURN:
        case SDL_SCANCODE_SPACE:
            new_cave();
            break;
        default:
            break;
        }
        break;
    }
}

void app_quit() {
    delete cave;
}

