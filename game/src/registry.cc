#include "context.hh"
#include "registry.hh"
#include "simulation.hh"
#include "types.hh"

static_assert(std::is_trivially_copy_constructible_v<RegistryReader>);
static_assert(std::is_trivially_copy_constructible_v<RegistryWriter>);

RegistryReader access_registry(ConstContext ctx) { return ctx->registry(); }

RegistryWriter access_registry(Context ctx) { return ctx->registry(); }
