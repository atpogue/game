#pragma once
#include "engine/core/handle.hh"
#include <SDL3/SDL_render.h>
#include <string_view>

struct Texture;

namespace textures {

    Handle<Texture> create(std::string_view path);
    Handle<Texture> create(SDL_Surface *surface);
    void destroy(Handle<Texture> handle);

    void quit();

} // namespace textures

