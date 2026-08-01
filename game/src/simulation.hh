#pragma once
#include "context.hh"

struct Simulation
{
  Catalog  catalog;
  Registry registry;
  Chunk    chunk; // TODO: multi-chunk world with lazy-loading

  /// Explciit copy to prevent unintended and expensive implicit copies.
  Simulation copy() const
  {
    return {
      .catalog  = catalog.copy(),
      .registry = registry.copy(),
      .chunk    = chunk,
    };
  }

  Context context() { return {.catalog = catalog, .registry = registry, .chunk = chunk}; }
};
