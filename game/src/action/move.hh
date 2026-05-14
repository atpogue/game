#include "engine/action.hh"
#include <glm/vec2.hpp>

struct MoveAction : Action {

    MoveAction(const Registry &r, Entity e, glm::vec2 direction, u8 duration);

    constexpr bool is_complete() const override { return duration_ == 0u; }
    constexpr bool cancel() override { return false; }

    void step(Registry &r, Entity e) override;

private:

    u8 duration_;
    const glm::vec2 to_, speed_;

};

