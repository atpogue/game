#include "app/state.hh"
#include "app/keyboard-pilot.hh"
#include "core/panic.hh"
#include "game/command-buffer.hh"
#include "game/world.hh"
#include "gfx/projection2D.hh"
#include "gfx/rectangle.hh"
#include "gfx/texture.hh"
#include "sys/event.hh"
#include <glm/common.hpp>

namespace App {
  Result<void> State::load(Game::World& world, Game::Entity player)
  {
    PRECONDITION(player != Game::Entity::Nil);
    player_ = {
      .entity = player,
      .pilot  = std::make_unique<KeyboardPilot>(),
      .camera = {
        .position = { 0.f, 0.f },
        .zoom     = 1.3f,
      },
    };
    {
      auto result = create_window("Game", 800, 600);
      if (!result) return Error(std::move(result).error());
      window_ = std::move(*result);
    }
    {
      auto result = create_renderer(window_);
      if (!result) return Error(std::move(result).error());
      renderer_ = std::move(*result);
    }
    if (!load_content("content/terrain.lua", world, renderer_, visuals_)) {
      return Error("failed to load content");
    }
    return {};
  }

  void State::handle_event(SDL_Event const& event)
  {
    switch (event.type) {
    case SDL_EVENT_KEY_DOWN:
      switch (event.key.scancode) {
      case SDL_SCANCODE_ESCAPE:
      case SDL_SCANCODE_Q:      push_event(make_quit_event()); return;
      default:                  break;
      }
      break;
    case SDL_EVENT_MOUSE_WHEEL:
      player_.camera.zoom = glm::clamp(player_.camera.zoom + 0.5f * event.wheel.y, 0.7f, 1.9f);
      return;
    default: break;
    }

    if (player_.pilot) player_.pilot->handle_event(event);
  }

  void State::step(Game::World const& world, Game::CommandBuffer& cmds)
  {
    if (player_.pilot) player_.pilot->steer(world, cmds, player_.entity);
    Handle<Game::Entity> const handle = world.find(player_.entity);
    if (!handle) return;
    if (auto pose = world.entities.try_get<Game::Pose>(handle)) {
      player_.camera.position = pose->position;
    }
  }

  void State::update(Game::World const&, f32) {}

  void State::render(Game::World const& world, f32)
  {
    constexpr Color    background = { 73, 49, 62, SDL_ALPHA_OPAQUE };
    Projection2D const project{
      .extent          = window_.size(),
      .pixels_per_unit = pixels_per_unit,
    };
    Rectangle const bounds = project.clip(player_.camera);

    renderer_.clear(background);

    glm::ivec2 lo = glm::ivec2(glm::floor(bounds.min()));
    glm::ivec2 hi = glm::ivec2(glm::ceil(bounds.max()));

    lo = glm::max(lo, glm::ivec2{ 0 });
    hi = glm::min(hi, glm::ivec2{ Game::chunk_size });

    for (i32 x = lo.x; x < hi.x; ++x) {
      for (i32 y = lo.y; y < hi.y; ++y) {
        Game::Tile const* tile = world.environment.get(u32(x), u32(y));
        if (!tile) continue;
        DEBUG_ASSERT(world.content.valid(tile->terrain));
        DEBUG_ASSERT(tile->terrain.index < visuals_.terrain.size());
        Sprite const& sprite  = visuals_.terrain[tile->terrain.index];
        Texture&      texture = visuals_.assets[sprite.atlas];
        if (!texture) continue;
        auto const      pixel = project.to_screen_space(player_.camera, { x, y });
        auto const      scale = player_.camera.zoom * pixels_per_unit;
        Rectangle const dst{ pixel, { scale, scale } };
        renderer_.draw_texture(texture, &sprite.source, &dst, &sprite.tint);
      }
    }
    renderer_.present();
  }
}
