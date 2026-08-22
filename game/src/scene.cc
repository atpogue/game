#include "game/scene.hh"
#include "game/chunk.hh"
#include "game/context.hh"
#include "game/simulation.hh"

Tile const* tile_at(ConstContext ctx, u32 x, u32 y) { return ctx->scene().get(x, y); }
