#include "action/move.hh"
#include "component/pose.hh"
#include "core/math.hh"
#include "core/random.hh"
#include "data.hh"
#include "director.hh"
#include "engine/event.hh"
#include "engine/main.hh"
#include "engine/render/camera.hh"
#include "engine/render/draw.hh"
#include "simulation.hh"
#include "world/grassland.hh"
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_render.h>
#include <random>

struct AppState
{
  Simulation sim;

  struct
  {
    Handle<Entity> entity;
    PlayerDirector director;
  } player;

  Camera camera;
};

void load_chunk(Context const ctx, Chunk& chunk)
{
  static u64 const                   seed = random_seed();
  static SplitMix64                  rng(seed);
  std::uniform_int_distribution<u32> dist{0u, chunk_size - 1u};
  GrasslandGenerator(ctx, seed).generate(dist(rng), dist(rng), chunk);
}

Handle<Entity> load_player(Context ctx)
{
  auto h = ctx.registry.create(Entity{0});
  ctx.registry.emplace<Pose>(h, Vec2f{chunk_size * 0.5f, chunk_size * 0.5f});
  DEBUG_ASSERT(ctx.registry.has<Pose>(h));
  return h;
}

void process_input(AppState& app)
{
  for (auto command : app.player.director.generate()) {
    try_submit_command(app.sim.context(), app.player.entity, command);
  }
}

AppConfig app_config()
{
  return {
    .step_rate = 32,
    .window    = {.title = "My Game", .width = 800, .height = 600, .min_zoom = 0.7f},
  };
}

AppState* app_start(int /*argc*/, char*[] /*argv*/)
{
  Simulation sim;
  if (!load_content(sim.catalog, "content/terrain.lua")) return nullptr;
  auto ctx = sim.context();
  load_chunk(ctx, sim.chunk);
  auto player = load_player(ctx);
  return new AppState{
    .sim    = std::move(sim),
    .player = {.entity = player,     .director = {}                       },
    .camera = {
               .position = {0.f, 0.f},
               .viewport = {800.f / tile_size, 600.f / tile_size},.zoom     = 1.3f,
               },
  };
}

void app_step(AppState& app)
{
  process_input(app);
  if (auto action = app.sim.registry.try_get<MoveAction>(app.player.entity)) {
    switch (act(app.sim.context(), app.player.entity, *action)) {
    case ActionResult::Canceled:
    case ActionResult::Complete: app.sim.registry.erase<MoveAction>(app.player.entity); break;
    case ActionResult::Ongoing:  break;
    }
  }
}

void app_update(AppState& app, nanoseconds)
{
  auto pose = app.sim.registry.try_get<Pose>(app.player.entity);
  if (pose) app.camera.position = pose->position;
}

void app_render(AppState& app)
{
  scene_begin();
  app.sim.chunk.render(app.sim.context(), app.camera, tile_size);
  scene_present(app.camera.zoom);
}

void app_event(AppState& app, SDL_Event const& event)
{
  app.player.director.event(event);
  switch (event.type) {
  case SDL_EVENT_MOUSE_WHEEL:
    app.camera.zoom = glm::clamp(app.camera.zoom + 0.5f * event.wheel.y, 0.7f, 1.9f);
    break;
  case SDL_EVENT_KEY_DOWN:
    switch (event.key.scancode) {
    case SDL_SCANCODE_ESCAPE:
    case SDL_SCANCODE_Q:      push_event(make_quit_event()); break;
    case SDL_SCANCODE_SPACE:  load_chunk(app.sim.context(), app.sim.chunk); break;
    default:                  break;
    }
    break;
  default: break;
  }
}

void app_quit(AppState* app) { delete app; }
