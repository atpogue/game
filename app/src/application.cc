#include "app/application.hh"
#include "app/keyboard.hh"
#include "core/panic.hh"
#include "game/catalog.hh"
#include "game/chunk.hh"
#include "game/command-buffer.hh"
#include "game/component/pose.hh"
#include "game/context.hh"
#include "game/entity.hh"
#include "game/scene.hh"
#include "game/texture.hh"
#include "game/types.hh"
#include "gfx/projection2D.hh"
#include "gfx/rectangle.hh"
#include "gfx/renderer.hh"
#include "gfx/surface.hh"
#include "sys/event.hh"
#include <expected>
#include <glm/common.hpp>
#include <memory>

// void scene_present(float zoom)
// {
//   SDL_SetRenderTarget(renderer, NULL);
//   SDL_SetRenderDrawColor(renderer, background.r, background.g, background.b, background.a);
//   SDL_RenderClear(renderer);

//   // Sample a centred sub-rectangle of size `window / zoom` and stretch it to
//   // fill the window: that stretch is the zoom.
//   SDL_FRect src = {
//     (scene_px.x - window_px.x / zoom) * 0.5f,
//     (scene_px.y - window_px.y / zoom) * 0.5f,
//     window_px.x / zoom,
//     window_px.y / zoom,
//   };
//   SDL_FRect dst = {0.f, 0.f, window_px.x, window_px.y};
//   SDL_RenderTexture(renderer, scene_target, &src, &dst);
// }
// scene_px  = glm::ceil(window_px / config.min_zoom);
// constexpr SDL_Color background = {73, 49, 62, SDL_ALPHA_OPAQUE};

Result<void> Application::load(LoadContext ctx, Entity player)
{
  PRECONDITION(player != Entity::Nil);
  player_ = {
    .entity = player,
    .pilot  = std::make_unique<KeyboardPilot>(),
    .camera = {
      .position = { 0.f, 0.f },
      .zoom     = 1.3f,
    },
  };
  {
    auto result = create_window("Game", 800, 600);
    if (!result) return std::unexpected(result.error());
    window_ = std::move(*result);
  }
  {
    auto result = create_renderer(window_);
    if (!result) return std::unexpected(result.error());
    renderer_ = std::move(*result);
  }
  auto catalog = access_catalog(ctx);
  textures_.reserve(catalog.count<TextureDef>());
  for (auto& asset : catalog.each<TextureDef>()) {
    // TODO: reuse the same surface, don't recreate a surface every time
    auto surface = load_image(asset.path);
    if (!surface) return std::unexpected(surface.error());
    auto texture = renderer_.create_texture(*surface);
    if (!texture) return std::unexpected(texture.error());
    textures_.push_back(std::move(*texture));
  }
  return {};
}

void Application::handle_event(SDL_Event const& event)
{
  switch (event.type) {
  case SDL_EVENT_KEY_DOWN:
    switch (event.key.scancode) {
    case SDL_SCANCODE_ESCAPE:
    case SDL_SCANCODE_Q:
      push_event(make_quit_event());
      return;
    default:
      break;
    }
    break;
  case SDL_EVENT_MOUSE_WHEEL:
    player_.camera.zoom = glm::clamp(player_.camera.zoom + 0.5f * event.wheel.y, 0.7f, 1.9f);
    return;
  default:
    break;
  }

  if (player_.pilot) player_.pilot->handle_event(event);
}

void Application::step(CommandBuffer& cmds, ConstContext ctx)
{
  if (player_.pilot) player_.pilot->steer(cmds, ctx, player_.entity);
  auto h = find_entity(ctx, player_.entity);
  if (!h) return;
  auto e    = access_entity(ctx, h);
  auto pose = e.try_get<Pose>();
  if (pose) player_.camera.position = pose->position;
}

void Application::update(ConstContext, f32) {}

void Application::render(ConstContext ctx, f32)
{
  constexpr Color    background = { 73, 49, 62, SDL_ALPHA_OPAQUE };
  Projection2D const project{
    .extent          = window_.size(),
    .pixels_per_unit = pixels_per_unit,
  };
  Rectangle const bounds = project.clip(player_.camera);

  renderer_.clear(background);

  auto catalog = access_catalog(ctx);

  glm::ivec2 lo = glm::ivec2(glm::floor(bounds.min()));
  glm::ivec2 hi = glm::ivec2(glm::ceil(bounds.max()));

  lo = glm::max(lo, glm::ivec2{ 0 });
  hi = glm::min(hi, glm::ivec2{ chunk_size });

  for (i32 x = lo.x; x < hi.x; ++x) {
    for (i32 y = lo.y; y < hi.y; ++y) {
      Tile const* tile = tile_at(ctx, u32(x), u32(y));
      if (!tile) continue;
      auto  pixel  = project.to_screen_space(player_.camera, { x, y });
      auto& sprite = catalog[tile->terrain].sprite;
      DEBUG_ASSERT(sprite.atlas.value < textures_.size());
      auto& texture = textures_[sprite.atlas.value];
      if (!texture) continue;
      auto      scale = player_.camera.zoom * pixels_per_unit;
      Rectangle dst{ pixel, { scale, scale } };
      renderer_.draw_texture(texture, &sprite.source, &dst, &sprite.tint);
    }
  }
  renderer_.present();
}

