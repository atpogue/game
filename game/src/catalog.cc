#include "catalog.hh"
#include "context.hh"
#include "simulation.hh"
#include "types.hh"

static_assert(std::is_trivially_copy_constructible_v<CatalogReader>);
static_assert(std::is_trivially_copy_constructible_v<CatalogWriter>);

CatalogReader access_catalog(ConstContext ctx) { return ctx->catalog(); }

CatalogWriter access_catalog(LoadContext ctx) { return ctx->catalog(); }

