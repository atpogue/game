#pragma once

#include <glm/ext/vector_float2.hpp>

struct SDL_Renderer;
struct SDL_Texture;

struct Texture
{
  Texture() noexcept                 = default;
  Texture(Texture const&)            = delete;
  Texture& operator=(Texture const&) = delete;

  Texture(SDL_Renderer*, SDL_Texture*) noexcept;
  Texture(Texture&&) noexcept;
  Texture& operator=(Texture&&) noexcept;
  ~Texture() noexcept;

  [[nodiscard]] bool valid() const noexcept { return handle_ != nullptr; }

  [[nodiscard]] SDL_Texture* handle() noexcept { return handle_; }

  [[nodiscard]] SDL_Renderer* owner() noexcept { return owner_; }

  glm::vec2 size() noexcept;

  [[nodiscard]] explicit operator bool() const noexcept { return valid(); }

private:
  SDL_Renderer* owner_  = nullptr;
  SDL_Texture*  handle_ = nullptr;
};

