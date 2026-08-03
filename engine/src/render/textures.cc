#include "engine/render/textures.hh"
#include "core/panic.hh"
#include "core/slot-map.hh"
#include "core/string.hh"
#include "internal.hh"
#include <SDL3/SDL_log.h>
#include <string_view>

struct Texture
{
  explicit Texture(SDL_Texture* t) : data{t} {}

  ~Texture() { SDL_DestroyTexture(data); }

  SDL_Texture* get() const { return data; }

private:

  SDL_Texture* data;
};

namespace { /////////////////////////////////////////////////////
  SlotMap<Texture>           textures;
  StringMap<Handle<Texture>> paths;
} // namespace

// needs happen before SDL quit
void destroy_all_textures() { textures.clear(); }

SDL_Texture* get_texture(Handle<Texture> handle)
{
  Texture* texture = textures.try_get(handle);
  return texture ? texture->get() : nullptr;
}

/// PUBLIC API //////////////////////////////////////////////////
Handle<Texture> create_texture(std::string_view path)
{
  if (auto it = paths.find(path); it != paths.end()) return it->second;
  SDL_Surface* surface = SDL_LoadPNG(path.data());
  if (!surface) {
    SDL_Log("Couldn't load png: %s", SDL_GetError());
    return Handle<Texture>::null();
  }
  auto handle = create_texture(surface);
  SDL_DestroySurface(surface);
  paths.emplace(path, handle);
  return handle;
}

Handle<Texture> create_texture(SDL_Surface* surface)
{
  SDL_Texture* texture = SDL_CreateTextureFromSurface(detail::get_renderer(), surface);
  if (!texture) {
    SDL_Log("Couldn't create static texture: %s", SDL_GetError());
    return Handle<Texture>::null();
  }
  SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
  auto handle = textures.emplace(texture);
  ASSERT(handle, "texture limit reached");
  return handle;
}

void destroy_texture(Handle<Texture> handle) { textures.erase(handle); }

void draw_texture(Handle<Texture> handle, Rectangle source, Rectangle dest, Color tint)
{
  auto         renderer = detail::get_renderer();
  SDL_Texture* texture  = get_texture(handle);
  PRECONDITION(texture, "texture handle must be valid");
  SDL_SetTextureColorMod(texture, tint.r, tint.g, tint.b);
  SDL_RenderTexture(renderer, texture, &source, &dest);
}
