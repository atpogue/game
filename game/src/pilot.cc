#include "pilot.hh"
#include "command-buffer.hh"
#include "command.hh"
#include "context.hh"
#include <SDL3/SDL_events.h>

void PlayerPilot::handle_event(SDL_Event const& event)
{
  mouse.event(event);
  keyboard.event(event);
}

void PlayerPilot::steer(CommandBuffer& out, ConstContext, Entity e)
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
