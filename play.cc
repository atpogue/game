#include "app/state.hh"
#include "core/clock.hh"
#include "core/panic.hh"
#include "core/random.hh"
#include "game/biome/grassland.hh"
#include "game/command-buffer.hh"
#include "game/world.hh"
#include "sys/main.hh"
#include <memory>
#include <print>
#include <random>

struct Runtime
{
  Game::World         world;
  App::State          app;
  Game::CommandBuffer cmds;
  Clock               clock = { 0, 32 };
};

static void generate_environment(Game::World& world)
{
  static u64 const                   seed = random_seed();
  static SplitMix64                  rng(seed);
  std::uniform_int_distribution<u32> dist{ 0u, Game::chunk_size - 1u };
  Game::GrasslandGenerator(world, seed).generate(dist(rng), dist(rng), world.environment);
}

static Game::Entity spawn_player(Game::World& world)
{
  Handle<Game::Entity> const handle = world.create();
  world.entities.emplace<Game::Pose>(
    handle, glm::vec2{ Game::chunk_size * 0.5f, Game::chunk_size * 0.5f });
  INVARIANT(world.find(world.entities[handle]) == handle);
  return world.entities[handle];
}

Runtime* start(int /*argc*/, char* /*argv*/[])
{
  auto state = std::make_unique<Runtime>();
  state->clock.set_rate(32);

  Game::Entity const player = spawn_player(state->world);
  if (auto result = state->app.load(state->world, player); !result) {
    std::println("Failed to open application: {}", result.error());
    return nullptr;
  }
  generate_environment(state->world);
  return state.release();
}

static void step(Runtime& state, i64 /*tick*/)
{
  state.app.step(state.world, state.cmds);
  state.cmds.dispatch(state.world);
  state.world.advance();
}

static void update(Runtime& state, f32 delta) { state.app.update(state.world, delta); }

static void render(Runtime& state, f32 alpha) { state.app.render(state.world, alpha); }

void iterate(Runtime& state)
{
  auto tick = state.clock.tick();
  for (auto steps = state.clock.advance(); steps > 0; steps--)
    step(state, tick++);
  DEBUG_ASSERT(tick == state.clock.tick());

  update(state, state.clock.delta());

  render(state, state.clock.alpha());
}

void handle_event(Runtime& state, SDL_Event const& event) { state.app.handle_event(event); }

void quit(Runtime* state) { delete state; }
