#pragma once
#include "app/pilot.hh"
#include "sys/keyboard.hh"
#include "sys/mouse.hh"

namespace App {
  struct KeyboardPilot : Pilot
  {
    void handle_event(SDL_Event const& event) override;
    void steer(Game::World const&, Game::CommandBuffer& out, Game::Entity entity) override;

  private:

    Keyboard keyboard_;
    Mouse    mouse_;
  };
}
