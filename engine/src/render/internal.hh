#pragma once
#include "core/handle.hh"
#include <glm/vec2.hpp>
struct Texture;
struct SDL_Texture;
struct SDL_Renderer;

namespace detail {
  SDL_Texture*  get_texture(Handle<Texture> handle);
  SDL_Renderer* get_renderer();

  /// Pixel dimensions of the off-screen "scene" render target. It is oversized
  /// (see `open_window`) so zooming out never reveals gaps; used to centre the
  /// world in the target. The public `scene_begin`/`scene_present` (draw.hh)
  /// drive rendering into and out of it.
  glm::vec2 scene_size();
} // namespace detail
