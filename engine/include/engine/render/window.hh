#pragma once
#include "core/types.hh"

struct WindowConfig
{
  char const* title;
  u16         width, height;
  // Minimum camera zoom the app will use. The off-screen scene target is
  // oversized to `window / min_zoom` so zooming out never reveals gaps.
  f32 min_zoom = 1.f;
};
