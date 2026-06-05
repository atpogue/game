#pragma once
#include "engine/core/catalog.hh"
#include "world/terrain.hh"

// TODO: registry like type-erased structure for definitions?
//      would allow callers do register their own definitions
//      without requiring knowledge of them in this file

// Immutable simulation state.
// Populated during loading.
struct GameDefinition { /////////////////////////////////////////////

    Catalog<Terrain> terrain;
    // TODO: Catalog<Item> items;
    // TODO: Catalog<Actor> actors;
    // TODO: Catalog<Structure> structures;

    // Explciit copy to prevent unintended and expensive implicit copies.
    GameDefinition copy() const {
        return {
            .terrain = terrain.copy(),
        };
    }

}; ///////////////////////////////////////////////////////////////////

