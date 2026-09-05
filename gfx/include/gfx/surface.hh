#pragma once
#include "core/result.hh"
#include <string_view>

struct Surface;

struct SDL_Surface;

[[nodiscard]] Result<Surface> load_image(std::string_view path);

struct Surface
{
  Surface() noexcept                 = default;
  Surface(Surface const&)            = delete;
  Surface& operator=(Surface const&) = delete;

  explicit Surface(SDL_Surface* data) noexcept;
  Surface(Surface&& other) noexcept;
  Surface& operator=(Surface&& other) noexcept;
  ~Surface() noexcept;

  [[nodiscard]] bool valid() const noexcept { return handle_ != nullptr; }

  [[nodiscard]] SDL_Surface* handle() noexcept { return handle_; }

  [[nodiscard]] explicit operator bool() const noexcept { return valid(); }

private:
  SDL_Surface* handle_ = nullptr;
};
