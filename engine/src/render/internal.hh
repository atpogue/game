#pragma once
#include "engine/core/handle.hh"

struct Texture;
struct SDL_Texture;
struct SDL_Renderer;

SDL_Texture *get_texture(Handle<Texture> handle);
SDL_Renderer *get_renderer();

