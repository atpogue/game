#include "context.hh"
#include "entity.hh"
#include "simulation.hh"
#include "types.hh"

static_assert(std::is_trivially_copy_constructible_v<EntityReader>);
static_assert(std::is_trivially_copy_constructible_v<EntityWriter>);

EntityWriter spawn(Context ctx)
{
  auto  id       = ctx->acquire_entity();
  auto& registry = ctx->registry();
  auto  handle   = registry.create(id);
  return EntityWriter(registry, handle);
}

EntityReader access_entity(ConstContext ctx, Handle<Entity> e)
{
  return EntityReader(ctx->registry(), e);
}

EntityWriter access_entity(Context ctx, Handle<Entity> e)
{
  return EntityWriter(ctx->registry(), e);
}

