#include "scene.hh"
#include "context.hh"
#include "simulation.hh"
#include "world/chunk.hh"

Tile const* tile_at(ConstContext ctx, u32 x, u32 y) { return ctx->scene().get(x, y); }
