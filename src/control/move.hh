#pragma once
#include "action.hh"
#include <glm/vec2.hpp>

struct MoveAction : Action {

    static bool is_compatible(Entity entity);

    MoveAction(glm::vec2 to);
    MoveAction(f32 x, f32 y);
    bool is_complete() const override;
    void step(Entity entity) override;
    bool cancel() override;

private:

    glm::vec2 direction;
    bool status = false;

};

/* TODO: pathfinding
struct PathToAction : Action {

    PathToAction(Vector2<float> from, Vector2<float> to, int duration)
        : step_(0u), duration_(duration)
        , from_(from), to_(to)
    { }

    Status status() const override { return step_ >= duration_; }
    Status cancel() const override { return true; }

    void step(Entity &entity) override {
        // TODO: pathfinding
        if (is_complete()) entity.position = to_;
        else entity.position = from_ + (to_ - from_) * float(step_number()) / float(duration());
        step_++;
    }

    Vector2<float> from() const { return from_; }
    Vector2<float> to()   const { return to_; }

private:

    Vector2<float> from_, to_;

};
*/

