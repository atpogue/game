#pragma once

// TODO: migration to SDL3 GPU for shader support
// TODO: headless renderer

#include "core/result.hh"
#include "gfx/color.hh"
#include "gfx/rectangle.hh"

struct Renderer;
struct Surface;
struct Texture;
struct Window;

struct SDL_Renderer;

[[nodiscard]] Result<Renderer> create_renderer(Window& window);

struct Renderer
{
  Renderer() noexcept                  = default;
  Renderer(Renderer const&)            = delete;
  Renderer& operator=(Renderer const&) = delete;

  explicit Renderer(SDL_Renderer*) noexcept;
  Renderer(Renderer&&) noexcept;
  Renderer& operator=(Renderer&&) noexcept;
  ~Renderer() noexcept;

  void draw_texture(
    Texture& texture, Rectangle const* src, Rectangle const* dst, Color const* tint) noexcept;

  [[nodiscard]] Result<Texture> create_texture(int width, int height);
  [[nodiscard]] Result<Texture> create_texture(Surface& surface);
  [[nodiscard]] Result<Texture> create_target(int width, int height);

  void set_target(Texture& texture) noexcept;
  void reset_target() noexcept;

  void clear(Color color) noexcept;
  void present() noexcept;

  [[nodiscard]] bool valid() const noexcept { return handle_ != nullptr; }

  [[nodiscard]] SDL_Renderer* handle() noexcept { return handle_; }

  [[nodiscard]] explicit operator bool() const noexcept { return valid(); }

private:
  SDL_Renderer* handle_ = nullptr;
};
