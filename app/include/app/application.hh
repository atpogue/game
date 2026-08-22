#pragma once
#include "core/error.hh"
#include "game/pilot.hh"
#include "gfx/camera2D.hh"
#include "gfx/renderer.hh"
#include "gfx/texture.hh"
#include "sys/window.hh"
#include <memory>
#include <vector>

union SDL_Event;
struct SDL_Texture;
struct CommandBuffer;
struct TextureDef;

struct Application
{
  Application()                                  = default;
  Application(Application&&) noexcept            = default;
  Application(Application const&)                = delete;
  Application& operator=(Application&&) noexcept = default;
  Application& operator=(Application const&)     = delete;

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

  Player               player_;
  Window               window_;
  Renderer             renderer_;
  std::vector<Texture> textures_;
};

