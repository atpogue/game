#pragma once
#include "core/types.hh"

struct Context;
struct Entity;

struct Command
{
  u32 id;
  enum class Type {
    Move,
    Path,
  } type;

  struct Move
  {
    f32 x, y;
  };
  struct Path
  {
    f32 x, y;
  };

  union
  { //////////////////////////////////////
    Move move;
    Path path;
  }; ///////////////////////////////////////////
};

Command make_move_command(u32 id, f32 x, f32 y);
Command make_path_command(u32 id, f32 x, f32 y);

// TODO: more informative command submission error (std::expected?)

// can be rejected if the entity doesn't have the components to execute the
// command
bool try_submit_command(Context ctx, Entity e, Command cmd);

