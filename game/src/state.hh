#pragma once
#include "context.hh"

// Mutable simulation state.
// Shouldn't be mutated outside of simulation systems and the command-action pipeline.
struct GameState { ///////////////////////////////////////////////////

    GameDefinition def;
    Registry<Components> entities;
    Chunk chunk; // TODO: multi-chunk world with lazy-loading

    // Explciit copy to prevent unintended and expensive implicit copies.
    GameState copy() const {
        return {
            .def = def.copy(),
            .entities = entities.copy(),
            .chunk = chunk,
        };
    }

    Context context() {
        return {
            .def = def,
            .entities = entities,
            .chunk = chunk
        };
    }

}; ///////////////////////////////////////////////////////////////////

