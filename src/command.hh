#pragma once
#include <vector>

union Command {

    enum class Type {
        Move,
        PathTo
    } type;

    struct {
        Type type;
        float x, y;
    } move;

    struct {
        Type type;
        float x, y;
    } path;

};

using CommandQueue = std::vector<Command>;

Command make_move_command(float x, float y);
Command make_path_command(float x, float y);

