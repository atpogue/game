#include "action/move.hh"
#include "component/pose.hh"
#include "context.hh"
#include "core/math.hh"
#include "core/random.hh"
#include "data.hh"
#include "director.hh"
#include "engine/event.hh"
#include "engine/main.hh"
#include "engine/render/camera.hh"
#include "engine/render/draw.hh"
#include "entity.hh"
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

void load_chunk(LoadContext ctx, Chunk& chunk)
{
  static u64 const                   seed = random_seed();
  static SplitMix64                  rng(seed);
  std::uniform_int_distribution<u32> dist{0u, chunk_size - 1u};
  GrasslandGenerator(ctx, seed).generate(dist(rng), dist(rng), chunk);
}

Handle<Entity> load_player(Context ctx)
{
  auto entity = spawn(ctx);
  entity.emplace<Pose>(Vec2f{chunk_size * 0.5f, chunk_size * 0.5f});
  DEBUG_ASSERT(entity.has<Pose>());
  return entity.handle();
}

void process_input(AppState& app, Context ctx)
{
  for (auto command : app.player.director.generate()) {
    try_submit_command(ctx, app.player.entity, command);
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
  auto       ctx = sim.load();
  if (!ctx) return nullptr;
  if (!load_content(*ctx, "content/terrain.lua")) return nullptr;
  load_chunk(*ctx, sim.scene());
  auto player = load_player(*ctx);
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
  auto ctx      = app.sim.step();
  auto registry = access_registry(ctx);
  process_input(app, ctx);
  if (auto action = registry.try_get<MoveAction>(app.player.entity)) {
    switch (act(ctx, app.player.entity, *action)) {
    case ActionResult::Canceled:
    case ActionResult::Complete: registry.erase<MoveAction>(app.player.entity); break;
    case ActionResult::Ongoing:  break;
    }
  }
}

void app_update(AppState& app, nanoseconds)
{
  auto ctx      = app.sim.context();
  auto registry = access_registry(ctx);
  auto pose     = registry.try_get<Pose>(app.player.entity);
  if (pose) app.camera.position = pose->position;
}

void app_render(AppState& app)
{
  auto ctx = app.sim.context();
  scene_begin();
  app.sim.scene().render(ctx, app.camera, tile_size);
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
    default:                  break;
    }
    break;
  default: break;
  }
}

void app_quit(AppState* app) { delete app; }
