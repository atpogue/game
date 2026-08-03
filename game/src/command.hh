#pragma once
#include "core/panic.hh"
#include "types.hh"
#include <cstddef>

enum class CommandError : u8 {
  None,
  BadCommand,
  DeadEntity,
  MissingComponent,
  Obstructed,
};

enum class CommandStatus : u8 {
  Pending,
  Accepted,
  Rejected,
};

// Disposition of a posted command: was it accepted into the simulation?
// This is *not* the action's outcome.
struct CommandReply
{
  CommandStatus status;
  CommandError  error = CommandError::None;
};

struct Command
{
  enum class Kind : u8 {
    Nil,
    Custom, // modder-defined command, interpret body (speculative)
    Move,
  };

  union Detail
  {
    struct Move
    {
      f32 x, y;
    } move;
  };

  Command() noexcept : actor_{Entity::Nil}, kind_{Kind::Nil}, body_{} {}

  Command(Entity e, Detail::Move detail) : Command(e, Kind::Move, Detail{detail}) {}

  Command(Command const&) noexcept            = default;
  Command(Command&&) noexcept                 = default;
  Command& operator=(Command&&) noexcept      = default;
  Command& operator=(Command const&) noexcept = default;
  ~Command() noexcept                         = default;

  [[nodiscard]] inline Detail::Move const& as_move() const
  {
    DEBUG_ASSERT(kind_ == Kind::Move);
    return detail_.move;
  }

  [[nodiscard]] inline Entity actor() const { return actor_; }

  [[nodiscard]] inline Kind kind() const { return kind_; }

  [[nodiscard]] inline bool is_empty() const { return kind_ != Kind::Nil; }

  [[nodiscard]] inline explicit operator bool() const { return !is_empty(); }

private:
  Command(Entity e, Kind kind, Detail detail) : actor_{e}, kind_{kind}, detail_{detail}
  {
    DEBUG_ASSERT(actor_ != Entity::Nil);
    DEBUG_ASSERT(kind_ != Kind::Nil);
  }

  Entity actor_ = Entity::Nil;
  Kind   kind_  = Kind::Nil;

  union
  {
    std::byte body_[sizeof(Detail)] = {}; // may hold anything or nothing
    Detail    detail_;
  };
};

[[nodiscard]] inline Command make_command_move(Entity e, f32 x, f32 y)
{
  return Command(e, Command::Detail::Move{x, y});
}

// Validate and translate a command into an action on its actor.
// Can be rejected if the entity doesn't have the components to execute the command.
CommandReply submit_command(Context ctx, Command const& cmd);

