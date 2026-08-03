#include "user-interface.hh"
#include "catalog.hh"
#include "command-buffer.hh"
#include "component/pose.hh"
#include "context.hh"
#include "core/panic.hh"
#include "engine/event.hh"
#include "engine/render/draw.hh"
#include "entity.hh"
#include "pilot.hh"
#include "scene.hh"
#include "world/tile.hh"
#include <glm/common.hpp>
#include <memory>

Result<void> UserInterface::load(ConstContext, Entity player)
{
  PRECONDITION(player != Entity::Nil);
  player_ = player;
  pilot_  = std::make_unique<PlayerPilot>();
  camera_ = {
    .position = {              0.f,               0.f},
    .viewport = {800.f / tile_size, 600.f / tile_size},
    .zoom     = 1.3f,
  };
  return {};
}

void UserInterface::handle_event(SDL_Event const& event)
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
    camera_.zoom = glm::clamp(camera_.zoom + 0.5f * event.wheel.y, 0.7f, 1.9f);
    return;
  default:
    break;
  }

  if (pilot_) pilot_->handle_event(event);
}

void UserInterface::step(CommandBuffer& cmds, ConstContext ctx)
{
  if (pilot_) pilot_->steer(cmds, ctx, player_);
  auto h = find_entity(ctx, player_);
  if (!h) return;
  auto e    = access_entity(ctx, h);
  auto pose = e.try_get<Pose>();
  if (pose) camera_.position = pose->position;
}

void UserInterface::update(ConstContext, nanoseconds) {}

void UserInterface::render(ConstContext ctx) const
{
  scene_begin();
  auto catalog = access_catalog(ctx);
  for (auto coord : camera_) {
    auto x    = u32(coord.x);
    auto y    = u32(coord.y);
    auto tile = tile_at(ctx, x, y);
    if (!tile) continue;
    auto pixel = camera_.view_coord_at({x, y}, tile_size);
    catalog[tile->terrain].sprite.draw(pixel.x, pixel.y, 1.0f);
  }
  scene_present(camera_.zoom);
}

