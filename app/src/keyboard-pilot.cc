#include "app/keyboard-pilot.hh"
#include "game/command-buffer.hh"
#include "game/command.hh"
#include <SDL3/SDL_events.h>

namespace App {
  void KeyboardPilot::handle_event(SDL_Event const& event)
  {
    mouse_.event(event);
    keyboard_.event(event);
  }

  void KeyboardPilot::steer(Game::World const&, Game::CommandBuffer& out, Game::Entity entity)
  {
    f32 x = 0.f, y = 0.f;
    if (keyboard_[SDL_SCANCODE_W]) y -= 0.1f;
    if (keyboard_[SDL_SCANCODE_S]) y += 0.1f;
    if (keyboard_[SDL_SCANCODE_A]) x -= 0.1f;
    if (keyboard_[SDL_SCANCODE_D]) x += 0.1f;
    if (x != 0.f || y != 0.f) out.post(Game::make_command_move(entity, x, y));
    mouse_.flush();
    keyboard_.flush();
  }
}
