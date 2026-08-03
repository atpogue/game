#pragma once

// TODO: polling of action results
// TODO: HerdPilot

#include "engine/input/keyboard.hh"
#include "engine/input/mouse.hh"
#include "types.hh"

union SDL_Event;
struct CommandBuffer;

// Generates commands to pass to Actors
struct Pilot
{
  virtual void handle_event(SDL_Event const& event)                  = 0;
  virtual void steer(CommandBuffer& out, ConstContext ctx, Entity e) = 0;
  virtual ~Pilot()                                                   = default;
};

struct PlayerPilot : Pilot
{
  void handle_event(SDL_Event const& event) override;
  void steer(CommandBuffer& out, ConstContext ctx, Entity e) override;

private:

  Keyboard keyboard;
  Mouse    mouse;
};
