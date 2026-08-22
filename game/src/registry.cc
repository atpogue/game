#include "game/registry.hh"
#include "game/context.hh"
#include "game/simulation.hh"
#include "game/types.hh"

static_assert(std::is_trivially_copy_constructible_v<RegistryReader>);
static_assert(std::is_trivially_copy_constructible_v<RegistryWriter>);

RegistryReader access_registry(ConstContext ctx) { return ctx->registry(); }

RegistryWriter access_registry(Context ctx) { return ctx->registry(); }
