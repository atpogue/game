#pragma once
#include "control/command.hh"
#include <generator>
#include <SDL3/SDL_events.h>

// Generates commands to pass to Actors
struct Director {
    virtual void event(const SDL_Event &e) = 0;
    virtual std::generator<Command> generate() = 0;
    virtual ~Director() = default;
};

