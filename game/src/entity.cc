#include "game/entity.hh"
#include "core/panic.hh"
#include "game/context.hh"
#include "game/simulation.hh"
#include "game/types.hh"

static_assert(std::is_trivially_copy_constructible_v<EntityReader>);
static_assert(std::is_trivially_copy_constructible_v<EntityWriter>);

EntityWriter create_entity(Context ctx)
{
  auto handle = ctx->create();
  INVARIANT(who(ctx, handle) != Entity::Nil);
  return EntityWriter(ctx->registry(), handle);
}

EntityReader access_entity(ConstContext ctx, Handle<Entity> e)
{
  PRECONDITION(ctx->registry().valid(e));
  return EntityReader(ctx->registry(), e);
}

EntityWriter access_entity(Context ctx, Handle<Entity> e)
{
  PRECONDITION(ctx->registry().valid(e));
  return EntityWriter(ctx->registry(), e);
}

