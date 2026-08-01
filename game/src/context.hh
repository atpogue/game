#pragma once
#include "action/move.hh"
#include "component/pose.hh"
#include "core/basic-catalog.hh"
#include "core/basic-registry.hh"
#include "types.hh"
#include "world/chunk.hh"
#include "world/terrain.hh"

struct Context
{
  Catalog const& catalog;
  Registry&      registry;
  Chunk&         chunk;
};
