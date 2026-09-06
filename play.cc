#include "app/application.hh"
#include "core/clock.hh"
#include "core/random.hh"
#include "game/biome/grassland.hh"
#include "game/command-buffer.hh"
#include "game/entity.hh"
#include "game/load.hh"
#include "game/simulation.hh"
#include "sys/main.hh"
#include <print>

struct Runtime
{
  Simulation    sim;
  Application   app;
  CommandBuffer cmds;
  Clock         clock = { 0, 32 };
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

Runtime* start(int /*argc*/, char* /*argv*/[])
{
  auto state = std::make_unique<Runtime>();
  state->clock.set_rate(32);

  auto ctx = state->sim.load();
  if (!ctx) return nullptr;
  if (!load_content(*ctx, "content/terrain.lua")) {
    std::println("Failed to load content.");
    return nullptr;
  }
  load_chunk(*ctx, state->sim.scene());
  if (auto result = state->app.load(*ctx, load_player(*ctx)); !result) {
    std::println("Failed to open application: {}", result.error());
    return nullptr;
  }
  return state.release();
}

void step(Runtime& state, i64 tick)
{
  auto ctx = state.sim.step(tick);
  state.app.step(state.cmds, ctx);
  state.cmds.dispatch(ctx);
}

void update(Runtime& state, f32 delta)
{
  auto ctx = state.sim.context();
  state.app.update(ctx, delta);
}

void render(Runtime& state, f32 alpha)
{
  auto ctx = state.sim.context();
  state.app.render(ctx, alpha);
}

void iterate(Runtime& state)
{
  auto tick = state.clock.tick();
  for (auto steps = state.clock.advance(); steps > 0; steps--) step(state, tick++);
  DEBUG_ASSERT(tick == state.clock.tick());

  update(state, state.clock.delta());

  render(state, state.clock.alpha());
}

void handle_event(Runtime& state, SDL_Event const& event) { state.app.handle_event(event); }

void quit(Runtime* state) { delete state; }
