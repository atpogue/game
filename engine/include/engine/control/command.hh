#pragma once
#include "engine/core/types.hh"

struct Command {
    u32 id;
    enum class Type { Move, Path, } type;

    struct Move { f32 x, y; };
    struct Path { f32 x, y; };

    union { //////////////////////////////////////
        Move move;
        Path path;
    }; ///////////////////////////////////////////
};

Command make_move_command(u32 id, f32 x, f32 y);
Command make_path_command(u32 id, f32 x, f32 y);

