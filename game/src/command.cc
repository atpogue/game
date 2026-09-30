#include "game/command.hh"
#include "game/world.hh"
#include <glm/geometric.hpp>
#include <type_traits>

namespace Game {
  static_assert(std::is_trivially_copy_constructible_v<Command>);
  static_assert(std::is_trivially_copy_assignable_v<Command>);

  static CommandReply resolve(World& world, Entity actor, Command::Detail::Move const& move)
  {
    auto handle = world.find(actor);
    if (!handle) return { CommandStatus::Rejected, CommandError::DeadEntity };
    auto pose = world.entities.try_get<Pose>(handle);
    if (!pose) return { CommandStatus::Rejected, CommandError::MissingComponent };
    pose->position += glm::normalize(glm::vec2(move.x, move.y));
    return { CommandStatus::Accepted };
  }

  CommandReply submit_command(World& world, Command const& cmd)
  {
    switch (cmd.kind()) {
    case Command::Kind::Nil:
    case Command::Kind::Custom: return { CommandStatus::Rejected, CommandError::BadCommand };
    case Command::Kind::Move:   return resolve(world, cmd.actor(), cmd.as_move());
    }
    std::unreachable();
  }
}
