#pragma once
#include "core/handle.hh"

struct Texture;
struct SDL_Texture;
struct SDL_Renderer;

namespace detail {

  SDL_Texture*  get_texture(Handle<Texture> handle);
  SDL_Renderer* get_renderer();

} // namespace detail

