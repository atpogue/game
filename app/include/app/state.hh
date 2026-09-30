#pragma once
#include "app/content.hh"
#include "app/pilot.hh"
#include "core/result.hh"
#include "game/entity.hh"
#include "gfx/camera2D.hh"
#include "gfx/renderer.hh"
#include "sys/window.hh"
#include <memory>

union SDL_Event;

namespace Game {
  struct CommandBuffer;
  struct World;
}

namespace App {
  struct Player
  {
    Game::Entity           entity = Game::Entity::Nil;
    std::unique_ptr<Pilot> pilot;
    Camera2D               camera;
  };

  struct State
  {
    State()                            = default;
    State(State&&) noexcept            = default;
    State(State const&)                = delete;
    State& operator=(State&&) noexcept = default;
    State& operator=(State const&)     = delete;

    // Opens the window and loads content into the world.
    [[nodiscard]] Result<void> load(Game::World&, Game::Entity player);
    void                       handle_event(SDL_Event const&);
    void                       step(Game::World const&, Game::CommandBuffer&);
    void                       update(Game::World const&, f32 delta);
    void                       render(Game::World const&, f32 alpha);

  private:

    // Declaration order matters: visuals must be destroyed before the renderer, and the renderer
    // before the window.
    Player   player_;
    Window   window_;
    Renderer renderer_;
    Visuals  visuals_;
  };
}
