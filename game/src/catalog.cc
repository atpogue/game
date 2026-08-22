#include "game/catalog.hh"
#include "game/context.hh"
#include "game/simulation.hh"
#include "game/types.hh"

static_assert(std::is_trivially_copy_constructible_v<CatalogReader>);
static_assert(std::is_trivially_copy_constructible_v<CatalogWriter>);

CatalogReader access_catalog(ConstContext ctx) { return ctx->catalog(); }

CatalogWriter access_catalog(LoadContext ctx) { return ctx->catalog(); }

