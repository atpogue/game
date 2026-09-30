#pragma once
#include "game/entity.hh"

union SDL_Event;

namespace Game {
  struct CommandBuffer;
  struct World;
}

namespace App {
  // Generates commands on behalf of an entity.
  struct Pilot
  {
    virtual void handle_event(SDL_Event const& event)                                 = 0;
    virtual void steer(Game::World const&, Game::CommandBuffer&, Game::Entity entity) = 0;
    virtual ~Pilot()                                                                  = default;
  };
}
