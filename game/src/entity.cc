#include "entity.hh"
#include "context.hh"
#include "core/panic.hh"
#include "simulation.hh"
#include "types.hh"

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

