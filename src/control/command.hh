#pragma once
#include <vector>

struct Command {
    enum class Type { Move, PathTo, } type;

    union { //////////////////////////////////////
        struct { float x, y; } move;
        struct { float x, y; } path;
    }; ///////////////////////////////////////////
};

using CommandQueue = std::vector<Command>;

Command move_command(float x, float y);
Command path_command(float x, float y);

