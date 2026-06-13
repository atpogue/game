#include "engine/time.hh"
#include <SDL3/SDL_events.h>
////////////////////////////////////////////////////////////////////////////////
// These are internally defined in different compilation units.
void destroy_all_textures();

namespace detail { /////////////////////////////////////////////////////////////

  // initialize all subsystems
  bool engine_init() { return true; }

  // progress all subsystems forwards one simulation step
  void engine_step() {}

  // progress animations, effects, and user-interface elements
  void engine_update(nanoseconds /*dt*/) {}

  // dispatch events to interested subsystems
  void engine_event(SDL_Event const& event)
  {
    switch (event.type) {
    case SDL_EVENT_WINDOW_FOCUS_LOST: SDL_ResetKeyboard(); break;
    default:                          break;
    }
  }

  // clear all subsystems here
  void engine_quit() { destroy_all_textures(); }
} // namespace detail
