#pragma once
#include "core/handle.hh"
#include "engine/render/draw.hh"
#include <SDL3/SDL_render.h>
#include <string_view>

struct Texture;

Handle<Texture> create_texture(std::string_view path);
Handle<Texture> create_texture(SDL_Surface* surface);

// TODO: Handle<Texture> get_or_create_texture(std::string_view path);
// TODO: Handle<Texture> get_texture(std::string_view path);

void destroy_texture(Handle<Texture> handle);

void draw_texture(Handle<Texture> texture, Rectangle source, Rectangle dest, Color tint);

