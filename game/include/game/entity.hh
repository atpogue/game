#pragma once
#include "core/types.hh"

namespace Game {
  // Goal: stable, persistent, cross-session, unique identifier for a simulation object.
  enum class Entity : u64 { Nil = UINT64_MAX };
}
