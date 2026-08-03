#include "engine/main.hh"
#include "command-buffer.hh"
#include "component/pose.hh"
#include "context.hh"
#include "core/math.hh"
#include "core/random.hh"
#include "data.hh"
#include "entity.hh"
#include "simulation.hh"
#include "types.hh"
#include "user-interface.hh"
#include "world/grassland.hh"
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_render.h>
#include <random>

struct AppState
{
  Simulation    sim;
  UserInterface ui;
  CommandBuffer cmds;
};

void load_chunk(LoadContext ctx, Chunk& chunk)
{
  static u64 const                   seed = random_seed();
  static SplitMix64                  rng(seed);
  std::uniform_int_distribution<u32> dist{0u, chunk_size - 1u};
  GrasslandGenerator(ctx, seed).generate(dist(rng), dist(rng), chunk);
}

Entity load_player(Context ctx)
{
  auto player = create_entity(ctx);
  player.emplace<Pose>(Vec2f{chunk_size * 0.5f, chunk_size * 0.5f});
  INVARIANT(find_entity(ctx, player.id()) != Handle<Entity>::null());
  return player.id();
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
  auto app = std::make_unique<AppState>();
  auto ctx = app->sim.load();
  if (!ctx) return nullptr;
  if (!load_content(*ctx, "content/terrain.lua")) return nullptr;
  load_chunk(*ctx, app->sim.scene());
  if (!app->ui.load(*ctx, load_player(*ctx))) return nullptr;
  return app.release();
}

void app_step(AppState& app)
{
  auto ctx = app.sim.step();
  app.ui.step(app.cmds, ctx);
  app.cmds.dispatch(ctx);

  /* TODO: pathfinding
  for (auto& [e, path, pose] : registry.query<PathAction, Pose>()) {
    path.act(e, pose);
    switch (path.status()) {
    case ActionResult::Canceled:
    case ActionResult::Complete:
      registry.erase<PathAction>(e);
      break;
    case ActionResult::Ongoing:
      break;
    }
  }
  */
}

void app_update(AppState& app, nanoseconds dt)
{
  auto ctx = app.sim.context();
  app.ui.update(ctx, dt);
}

void app_render(AppState& app)
{
  auto ctx = app.sim.context();
  app.ui.render(ctx);
}

void app_event(AppState& app, SDL_Event const& event) { app.ui.handle_event(event); }

void app_quit(AppState* app) { delete app; }
