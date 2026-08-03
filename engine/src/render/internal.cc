#include "internal.hh"
#include "engine/render/draw.hh"
#include "engine/render/window.hh"
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_render.h>
#include <glm/common.hpp>

namespace {
  SDL_Window*   window;
  SDL_Renderer* renderer;
  SDL_Texture*  scene_target;
  glm::vec2     window_px;
  glm::vec2     scene_px;

  // Background fill, also shown in any letterbox region.
  constexpr SDL_Color background = {73, 49, 62, SDL_ALPHA_OPAQUE};
} // namespace

namespace detail {
  SDL_Renderer* get_renderer() { return renderer; }

  bool open_window(WindowConfig config)
  {
    window = SDL_CreateWindow(config.title, config.width, config.height, 0);
    if (!window) {
      SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not create window: %s\n", SDL_GetError());
      return false;
    }
    renderer = SDL_CreateRenderer(window, NULL);
    if (!renderer) {
      SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not create renderer: %s\n", SDL_GetError());
      return false;
    }

    // The scene target must be large enough to cover the most zoomed-out view
    // (`window / min_zoom`) so the blit in `scene_present` never samples beyond
    // its bounds.
    window_px = {float(config.width), float(config.height)};
    scene_px  = glm::ceil(window_px / config.min_zoom);

    scene_target = SDL_CreateTexture(
      renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, int(scene_px.x), int(scene_px.y)
    );
    if (!scene_target) {
      SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not create scene target: %s\n", SDL_GetError());
      return false;
    }
    SDL_SetTextureScaleMode(scene_target, SDL_SCALEMODE_NEAREST); // crisp pixel-art
    return true;
  }

  void close_window()
  {
    SDL_DestroyTexture(scene_target);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
  }

  glm::vec2 scene_size() { return scene_px; }
} // namespace detail

// Public render API (declared in engine/render/draw.hh). Only the game drives
// the scene, so these live in the global namespace rather than `detail`.
void scene_begin()
{
  SDL_SetRenderTarget(renderer, scene_target);
  SDL_SetRenderDrawColor(renderer, background.r, background.g, background.b, background.a);
  SDL_RenderClear(renderer);
}

void scene_present(float zoom)
{
  SDL_SetRenderTarget(renderer, NULL);
  SDL_SetRenderDrawColor(renderer, background.r, background.g, background.b, background.a);
  SDL_RenderClear(renderer);

  // Sample a centred sub-rectangle of size `window / zoom` and stretch it to
  // fill the window: that stretch is the zoom.
  SDL_FRect src = {
    (scene_px.x - window_px.x / zoom) * 0.5f,
    (scene_px.y - window_px.y / zoom) * 0.5f,
    window_px.x / zoom,
    window_px.y / zoom,
  };
  SDL_FRect dst = {0.f, 0.f, window_px.x, window_px.y};
  SDL_RenderTexture(renderer, scene_target, &src, &dst);
}
