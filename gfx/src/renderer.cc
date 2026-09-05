#include "gfx/renderer.hh"
#include "core/panic.hh"
#include "gfx/surface.hh"
#include "gfx/texture.hh"
#include "sys/window.hh"
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_surface.h>
#include <expected>
#include <print>

static void to_sdl_rect(SDL_FRect& to, Rectangle const& from) noexcept
{
  to.x = from.x();
  to.y = from.y();
  to.w = from.width();
  to.h = from.height();
}

Renderer::Renderer(SDL_Renderer* handle) noexcept : handle_{ handle }
{
  PRECONDITION(handle != nullptr);
}

Renderer::Renderer(Renderer&& other) noexcept : handle_{ other.handle_ }
{
  other.handle_ = nullptr;
}

Renderer& Renderer::operator=(Renderer&& other) noexcept
{
  if (&other == this) return *this;
  handle_       = other.handle_;
  other.handle_ = nullptr;
  return *this;
}

Renderer::~Renderer() noexcept { SDL_DestroyRenderer(handle_); }

Result<Renderer> create_renderer(Window& window)
{
  SDL_Renderer* handle = SDL_CreateRenderer(window.handle(), nullptr);
  if (!handle) return Error(SDL_GetError());
  if (!SDL_SetRenderVSync(handle, 1))
    std::println("[Warning] Could not enable vsync: %s", SDL_GetError());
  return Renderer(handle);
}

void Renderer::draw_texture(
  Texture& texture, Rectangle const* src, Rectangle const* dst, Color const* tint) noexcept
{
  DEBUG_ASSERT(texture.owner() == handle_);

  SDL_FRect  s, d;
  SDL_FRect* from = NULL;
  SDL_FRect* to   = NULL;

  if (src) {
    to_sdl_rect(s, *src);
    from = &s;
  }

  if (dst) {
    to_sdl_rect(d, *dst);
    to = &d;
  }

  if (tint) {
    SDL_SetTextureColorMod(texture.handle(), tint->r, tint->g, tint->b);
    SDL_SetTextureAlphaMod(texture.handle(), tint->a);
  }

  SDL_RenderTexture(handle_, texture.handle(), from, to);
}

Result<Texture> Renderer::create_texture(int w, int h)
{
  SDL_Texture* texture
    = SDL_CreateTexture(handle_, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STATIC, w, h);
  if (!texture) return Error(SDL_GetError());
  if (!SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST))
    std::println("[Warning] Could not set scale mode: %s", SDL_GetError());
  return Texture(handle_, texture);
}

Result<Texture> Renderer::create_texture(Surface& surface)
{
  SDL_Texture* texture = SDL_CreateTextureFromSurface(handle_, surface.handle());
  if (!texture) return Error(SDL_GetError());
  if (!SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST))
    std::println("[Warning] Could not set scale mode: %s", SDL_GetError());
  return Texture(handle_, texture);
}

Result<Texture> Renderer::create_target(int width, int height)
{
  auto target
    = SDL_CreateTexture(handle_, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);
  if (!target) return Error(SDL_GetError());
  if (!SDL_SetTextureScaleMode(target, SDL_SCALEMODE_NEAREST))
    std::println("[Warning] Could not set scale mode: %s", SDL_GetError());
  return Texture(handle_, target);
}

void Renderer::set_target(Texture& texture) noexcept
{
  SDL_SetRenderTarget(handle_, texture.handle());
}

void Renderer::reset_target() noexcept { SDL_SetRenderTarget(handle_, NULL); }

void Renderer::clear(Color color) noexcept
{
  SDL_SetRenderDrawColor(handle_, color.r, color.g, color.b, color.a);
  SDL_RenderClear(handle_);
}

void Renderer::present() noexcept { SDL_RenderPresent(handle_); }

// void renderer::draw_rectangle(rectangle& rect, color color)
// {
//   sdl_frect r;
//   to_sdl_rect(r, rect);
//   sdl_setrenderdrawcolor(handle_, color.r, color.g, color.b, color.a);
//   sdl_renderfillrect(handle_, &r);
// }
