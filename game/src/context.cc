#include "game/context.hh"
#include "game/simulation.hh"
#include <type_traits>

static_assert(std::is_trivially_copy_constructible_v<Context>);
static_assert(std::is_trivially_copy_constructible_v<ConstContext>);
static_assert(std::is_trivially_copy_constructible_v<LoadContext>);

Entity who(ConstContext ctx, Handle<Entity> handle)
{
  auto& reg = ctx->registry();
  return reg.valid(handle) ? reg[handle] : Entity::Nil;
}

Handle<Entity> find_entity(ConstContext ctx, Entity id) { return ctx->find(id); }

bool is_valid(ConstContext ctx, Handle<Entity> handle) { return ctx->registry().valid(handle); }

