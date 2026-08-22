#pragma once
#include "game/types.hh"

union SDL_Event;
struct CommandBuffer;

// Generates commands to pass to Actors
struct Pilot
{
  virtual void handle_event(SDL_Event const& event)                  = 0;
  virtual void steer(CommandBuffer& out, ConstContext ctx, Entity e) = 0;
  virtual ~Pilot()                                                   = default;
};

