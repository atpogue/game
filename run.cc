#include "app/application.hh"
#include "core/random.hh"
#include "game/biome/grassland.hh"
#include "game/command-buffer.hh"
#include "game/component/pose.hh"
#include "game/context.hh"
#include "game/entity.hh"
#include "game/load.hh"
#include "game/simulation.hh"
#include "game/types.hh"
#include "sys/main.hh"
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_render.h>
#include <print>
#include <random>

struct AppState
{
  Simulation    sim;
  Application   app;
  CommandBuffer cmds;
};

void load_chunk(LoadContext ctx, Chunk& chunk)
{
  static u64 const                   seed = random_seed();
  static SplitMix64                  rng(seed);
  std::uniform_int_distribution<u32> dist{ 0u, chunk_size - 1u };
  GrasslandGenerator(ctx, seed).generate(dist(rng), dist(rng), chunk);
}

Entity load_player(Context ctx)
{
  auto player = create_entity(ctx);
  player.emplace<Pose>(glm::vec2{ chunk_size * 0.5f, chunk_size * 0.5f });
  INVARIANT(find_entity(ctx, player.id()) != Handle<Entity>::null());
  return player.id();
}

AppConfig configure(int /*argc*/, char*[] /*argv*/)
{
  return {
    .step_rate = 32,
  };
}

AppState* start()
{
  auto state = std::make_unique<AppState>();
  auto ctx   = state->sim.load();
  if (!ctx) return nullptr;
  if (!load_content(*ctx, "content/terrain.lua")) {
    std::println("Failed to load content.");
    return nullptr;
  }
  load_chunk(*ctx, state->sim.scene());
  if (auto result = state->app.load(*ctx, load_player(*ctx)); !result) {
    std::println("Failed to open application: {}", result.error().msg);
    return nullptr;
  }
  return state.release();
}

void step(AppState& state)
{
  auto ctx = state.sim.step();
  state.app.step(state.cmds, ctx);
  state.cmds.dispatch(ctx);

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

void update(AppState& state, f32 delta)
{
  auto ctx = state.sim.context();
  state.app.update(ctx, delta);
}

void render(AppState& state, f32 alpha)
{
  auto ctx = state.sim.context();
  state.app.render(ctx, alpha);
}

void handle_event(AppState& state, SDL_Event const& event) { state.app.handle_event(event); }

void quit(AppState* state) { delete state; }
