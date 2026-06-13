#pragma once
#include "world/terrain.hh"

#include "engine/core/catalog.hh"

// TODO: registry like type-erased structure for definitions?
//      would allow callers do register their own definitions
//      without requiring knowledge of them in this file

// A simulation rulebook that defines what exists and how what exists should
// behave.
struct Codex
{
  // Alternative names: Rulebook, GameContent, Resources

  Catalog<Terrain> terrain;
  // TODO: Catalog<Item> items;
  // TODO: Catalog<Actor> actors;
  // TODO: Catalog<Structure> structures;

  // Explciit copy to prevent unintended and expensive implicit copies.
  Codex copy() const
  {
    return {
      .terrain = terrain.copy(),
    };
  }
};

bool load_content(Codex& codex, std::string_view path);

