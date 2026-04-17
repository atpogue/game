#pragma once
#include "core/types.hh"
#include <vector>

struct Command {
    enum class Type { Move, PathTo, } type;

    union { //////////////////////////////////////
        struct { f32 x, y; } move;
        struct { f32 x, y; } path;
    }; ///////////////////////////////////////////
};

using CommandQueue = std::vector<Command>;

Command move_command(f32 x, f32 y);
Command path_command(f32 x, f32 y);

