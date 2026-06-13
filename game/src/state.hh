#pragma once
#include "context.hh"

/// Mutable simulation state.
/// Shouldn't be mutated outside of simulation systems and the command-action
/// pipeline.
struct GameState
{ /////////////////////////////////////////////////////////////////////////////////////////////////
  Codex                codex;
  Registry<Components> entities;
  Chunk                chunk; // TODO: multi-chunk world with lazy-loading

  /// Explciit copy to prevent unintended and expensive implicit copies.
  GameState copy() const
  {
    return {
      .codex    = codex.copy(),
      .entities = entities.copy(),
      .chunk    = chunk,
    };
  }

  Context context() { return {.codex = codex, .entities = entities, .chunk = chunk}; }
}; ////////////////////////////////////////////////////////////////////////////////////////////////
