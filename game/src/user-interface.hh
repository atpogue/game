#pragma once
#include "core/error.hh"
#include "engine/render/camera.hh"
#include "engine/time.hh"
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

  Result<void> load(ConstContext ctx, Entity player);
  void         handle_event(SDL_Event const& event);
  void         step(CommandBuffer& cmds, ConstContext ctx);
  void         update(ConstContext ctx, nanoseconds dt);
  void         render(ConstContext ctx) const;

private:
  Entity                 player_;
  std::unique_ptr<Pilot> pilot_;
  Handle<SDL_Texture>    gui_, scene_;
  Camera                 camera_;
};

