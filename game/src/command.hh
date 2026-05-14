#pragma once
#include "engine/action.hh"
#include "engine/core/types.hh"
#include "engine/registry.hh"
#include <memory>

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

std::unique_ptr<Action> make_action(const Registry &registry, Entity e, const Command &cmd);
std::unique_ptr<Action> make_action(const Registry &registry, Entity e, const Command::Move &move);
constexpr std::unique_ptr<Action> make_action(const Registry &, Entity, const Command::Path &) { return nullptr; }

