#include "control/actor.hh"
#include "control/director.hh"
#include "core/random.hh"
#include "core/event.hh"
#include "ecs/ledger.hh"
#include "ecs/pose.hh"
#include "render/camera.hh"
#include "render/textures.hh"
#include "render/window.hh"
#include "world/chunk.hh"
#include <cassert>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_render.h>

namespace { ///////////////////////////////////////////////////////////////////////////////

    constexpr f32 tile_size = 16.f;

    Chunk chunk;
    Camera camera;

    struct {
        Entity entity;
        PlayerDirector director;
    } player;

} /////////////////////////////////////////////////////////////////////////////////////////

WindowConfig app_window_config() {
    return { .title = "My Game", .width = 800, .height = 600 };
}

static bool create_player() {
    auto e = ledger::create();
    assert(e && ledger::status(e));
    player.entity = e;

    auto pose = poses::set(e, { .position = {chunk_size / 2.f, chunk_size /2.f} });
    assert(pose && poses::get(e));
    assert(ledger::has(e, Flag::Pose));

    actors::create(e, 2);
    assert(ledger::has(e, Flag::Actor));

    return true;
}

static bool load_terrain() {
    auto atlas = textures::create("assets/kenney-1bitpack.png");
    assert(atlas);
    auto sprite = [atlas](f32 x, f32 y, Color color) {
        return Sprite{atlas, {x * tile_size, y * tile_size, tile_size, tile_size}, color};
    };
    terrains::create("grass-1", sprite(5,0,Color{59,216,114}));
    terrains::create("grass-2", sprite(6,0,Color{59,216,114}));
    terrains::create("grass-3", sprite(7,0,Color{59,216,114}));
    terrains::create("grass-tall", sprite(0,2,Color{59,216,114}));
    terrains::create("dirt", sprite(2,0,Color{121,70,75}));
    terrains::create("rocks", sprite(2,0,Color{206,197,183}));
    return true;
}

static bool load_world() {
    if (!generate_chunk(random_seed(), 2, 3, chunk)) return false;
    return true;
}

bool app_init() {
    camera.viewport = {800/tile_size, 600/tile_size};
    return load_terrain()
        && create_player()
        && load_world();
}

void app_step() {
    for (auto command : player.director.generate())
        actors::act(player.entity, command);
}

void app_update(float dt) {
}

void app_render() {
    auto pose = poses::get(player.entity);
    if (pose) camera.position = pose->position;
    chunk.render(camera, tile_size);
}

void app_event(const SDL_Event &event) {
    player.director.event(event);
    switch (event.type) {
    case SDL_EVENT_KEY_DOWN:
        if (event.key.scancode != SDL_SCANCODE_ESCAPE
        &&  event.key.scancode != SDL_SCANCODE_Q)
            break;
        if (!push_event(make_quit_event()))
        break;
    }
}

void app_quit() {
}

