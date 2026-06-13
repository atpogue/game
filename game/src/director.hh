#pragma once
#include "command.hh"
#include "engine/input/keyboard.hh"
#include "engine/input/mouse.hh"
#include <SDL3/SDL_events.h>
#include <generator>

// TODO: polling of action results
// Generates commands to pass to Actors
struct Director
{
  virtual void                    event(SDL_Event const& e) = 0;
  virtual std::generator<Command> generate()                = 0;
  virtual ~Director()                                       = default;
};

struct PlayerDirector : Director
{
  void                    event(SDL_Event const& e) override;
  std::generator<Command> generate() override;

private:

  Keyboard keyboard;
  Mouse    mouse;
  u32      next_id = 0u;
};
