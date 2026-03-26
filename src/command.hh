#pragma once
#include <vector>

union Command {

    enum class Type {
        Move,
    } type;

    struct Move {
        Type type;
        // TODO: Direction direction;
    } move;

    struct PathTo {
        Type type;
        float x, y;
    } path;

};

using CommandQueue = std::vector<Command>;

