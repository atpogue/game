#pragma once
#include "codex.hh"
#include "core/registry.hh"
#include "core/type-info.hh"
#include "world/chunk.hh"

struct Pose;
struct MoveAction;

// TODO: type info that doesn't require types to be known at compile-time
//  but still allows for determinism
using Components = TypeList<Pose, MoveAction>;

struct Context
{
  const Codex&          codex;
  Registry<Components>& entities;
  Chunk&                chunk;
};

