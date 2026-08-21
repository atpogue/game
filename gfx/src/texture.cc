#include "gfx/texture.hh"
#include "core/panic.hh"
#include "core/types.hh"
#include <SDL3/SDL_render.h>

Texture::Texture(SDL_Renderer* renderer, SDL_Texture* texture) noexcept
  : owner_{ renderer }, handle_{ texture }
{
  PRECONDITION(renderer != nullptr);
  PRECONDITION(texture != nullptr);
}

Texture::Texture(Texture&& other) noexcept : owner_{ other.owner_ }, handle_{ other.handle_ }
{
  other.handle_ = nullptr;
  other.owner_  = nullptr;
}

Texture& Texture::operator=(Texture&& other) noexcept
{
  if (this == &other) return *this;
  handle_       = other.handle_;
  owner_        = other.owner_;
  other.handle_ = nullptr;
  other.owner_  = nullptr;
  return *this;
}

Texture::~Texture() noexcept { SDL_DestroyTexture(handle_); }

glm::vec2 Texture::size() noexcept
{
  f32 w, h;
  SDL_GetTextureSize(handle_, &w, &h);
  return { w, h };
}

