#include "app/keyboard.hh"
#include "game/command-buffer.hh"
#include "game/command.hh"
#include "game/context.hh"
#include <SDL3/SDL_events.h>

void KeyboardPilot::handle_event(SDL_Event const& event)
{
  mouse.event(event);
  keyboard.event(event);
}

void KeyboardPilot::steer(CommandBuffer& out, ConstContext, Entity e)
{
  f32 x = 0.f, y = 0.f;
  if (keyboard[SDL_SCANCODE_W]) y -= 0.1f;
  if (keyboard[SDL_SCANCODE_S]) y += 0.1f;
  if (keyboard[SDL_SCANCODE_A]) x -= 0.1f;
  if (keyboard[SDL_SCANCODE_D]) x += 0.1f;
  if (x != 0.f || y != 0.f) out.post(make_command_move(e, x, y));
  mouse.flush();
  keyboard.flush();
}
