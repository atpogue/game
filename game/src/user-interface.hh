#pragma once
#include "app/window.hh"
#include "core/error.hh"
#include "core/slot-map.hh"
#include "gfx/camera2D.hh"
#include "gfx/renderer.hh"
#include "gfx/texture.hh"
#include "pilot.hh"
#include <memory>

union SDL_Event;
struct SDL_Texture;
struct CommandBuffer;

struct UserInterface
{
  UserInterface()                                    = default;
  UserInterface(UserInterface&&) noexcept            = default;
  UserInterface(UserInterface const&)                = delete;
  UserInterface& operator=(UserInterface&&) noexcept = default;
  UserInterface& operator=(UserInterface const&)     = delete;

  Result<void> load(LoadContext ctx, Entity player);
  void         handle_event(SDL_Event const& event);
  void         step(CommandBuffer& cmds, ConstContext ctx);
  void         update(ConstContext ctx, f32 dt);
  void         render(ConstContext ctx, f32 a);

private:
  struct Player
  {
    Entity                 entity;
    std::unique_ptr<Pilot> pilot;
    Camera2D               camera;
  };

  Player           player_;
  Window           window_;
  Renderer         renderer_;
  SlotMap<Texture> textures_;
};

