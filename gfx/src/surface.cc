#include "gfx/surface.hh"
#include "core/panic.hh"
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_surface.h>
#include <format>

Surface::Surface(SDL_Surface* handle) noexcept : handle_{ handle }
{
  PRECONDITION(handle != nullptr);
}

Surface::Surface(Surface&& other) noexcept : handle_{ other.handle_ } { other.handle_ = nullptr; }

Surface& Surface::operator=(Surface&& other) noexcept
{
  if (this == &other) return *this;
  handle_       = other.handle_;
  other.handle_ = nullptr;
  return *this;
}

Surface::~Surface() noexcept { SDL_DestroySurface(handle_); }

Result<Surface> load_image(std::string_view path)
{
  SDL_Surface* surface = nullptr;
  if (path.ends_with(".png")) surface = SDL_LoadPNG(std::string(path).c_str());
  else if (path.ends_with(".bmp")) surface = SDL_LoadBMP(std::string(path).c_str());
  else {
    return Error(
      std::format("Unsupported file type (expected BMP or PNG): {}", path));
  }
  if (!surface) { return Error(SDL_GetError()); }
  return Surface(surface);
}
