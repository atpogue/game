#pragma once
#include "action/move.hh"
#include "codex.hh"
#include "component/pose.hh"
#include "core/registry.hh"
#include "types.hh"
#include "world/chunk.hh"

struct Context
{
  Codex const& codex;
  Registry&    entities;
  Chunk&       chunk;
};
