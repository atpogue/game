#include "control/actor.hh"
#include "control/director.hh"
#include "core/random.hh"
#include "ecs/ledger.hh"
#include "ecs/pose.hh"
#include "render/graphics.hh"
#include "render/camera.hh"
#include "world/chunk.hh"
#include <cassert>
#include <cmath>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_render.h>

namespace { ///////////////////////////////////////////////////////////////////////////////

    // The length of a fixed time step in seconds
    constexpr f32 step_size = 1.f / 30.f;
    constexpr f32 tile_size = 16.f;

    f32 lag = 0.f;

    Chunk chunk;
    Camera camera;

    struct {
        Entity entity;
        PlayerDirector director;
    } player;

} /////////////////////////////////////////////////////////////////////////////////////////

// Get the number of seconds elapsed since SDL was initialized
static f32 time() { return (f32)(SDL_GetTicks()) / 1000.f; }

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
    auto atlas = graphics::create_texture("assets/kenney-1bitpack.png");
    assert(atlas && graphics::get_texture(atlas));
    auto sprite = [atlas](f32 x, f32 y, Color color) {
        return Sprite{atlas, {x * tile_size, y * tile_size, tile_size, tile_size}, color};
    };
    terrain::create("grass-1", sprite(5,0,Color{59,216,114}));
    terrain::create("grass-2", sprite(6,0,Color{59,216,114}));
    terrain::create("grass-3", sprite(7,0,Color{59,216,114}));
    terrain::create("grass-tall", sprite(0,2,Color{59,216,114}));
    terrain::create("dirt", sprite(2,0,Color{121,70,75}));
    terrain::create("rocks", sprite(2,0,Color{206,197,183}));
    return true;
}

static bool load_world() {
    if (!generate_chunk(random_seed(), 2, 3, chunk)) return false;
    return true;
}

bool app_init(SDL_Window *window, SDL_Renderer *renderer) {
    int window_width, window_height;
    SDL_GetWindowSize(window, &window_width, &window_height);
    camera.viewport = {window_width/tile_size, window_height/tile_size};
    return graphics::init(renderer)
        && load_terrain()
        && create_player()
        && load_world();
}

void app_update(float dt) {
    lag += dt;

    for (lag += dt; lag >= step_size; lag -= step_size) {
        // progress the simulation forwards one fixed time step
        // use a std::generator for commands
        for (auto command : player.director.generate())
            actors::act(player.entity, command);
        actors::step();
    }

    dt = fmin(dt, 0.25f);
}


void app_render(SDL_Renderer *renderer) {
    auto pose = poses::get(player.entity);
    if (pose) camera.position = pose->position;

    // draw the world
    for (auto world_coord : camera) {
        u32 x = world_coord.x, y = world_coord.y;
        const Tile *tile = chunk.get(x, y);
        if (!tile) continue;
        const Sprite &sprite = terrain::get(tile->terrain).sprite;
        SDL_Texture *texture = graphics::get_texture(sprite.atlas);
        assert(texture);
        //if (!texture) continue;
        auto coord = camera.view_coord_at(world_coord) * tile_size;
        auto scale = tile_size * camera.zoom;
        Rectangle destination{ coord.x, coord.y, scale, scale };
        SDL_SetTextureColorMod(texture, sprite.color.r, sprite.color.g, sprite.color.b);
        SDL_RenderTexture(renderer, texture, &sprite.source, &destination);
    }

    // draw the player
    if (pose) {
        auto size = glm::vec2{20.f, 20.f} * camera.zoom;
        auto coord = camera.view_coord_at(pose->position) * tile_size - size / 2.f;
        Rectangle player_square{ coord.x, coord.y, size.x, size.y };
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderFillRect(renderer, &player_square);
    }
}

void app_event(const SDL_Event &event) {
    // dispatch events to interested systems
    player.director.event(event);
}

void app_quit() {
    graphics::quit(); // destroy all the textures
}

