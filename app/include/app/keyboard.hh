#pragma once

// TODO: polling of action results
// TODO: HerdPilot

#include "game/pilot.hh"
#include "game/types.hh"
#include "sys/keyboard.hh"
#include "sys/mouse.hh"

struct KeyboardPilot : Pilot
{
  void handle_event(SDL_Event const& event) override;
  void steer(CommandBuffer& out, ConstContext ctx, Entity e) override;

private:

  Keyboard keyboard;
  Mouse    mouse;
};
