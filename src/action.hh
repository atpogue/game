#pragma once
#include "ledger.hh"
#include <glm/vec2.hpp>
#include <string>

struct Action {

    enum class Status {
        Success,    // completed without issue
        Fail,       // could not complete
        Busy,       // in-progress, not yet complete
        Suspended,  // paused, can be resumed
        Pending,    // hasn't started yet
        Expired,    // no longer valid
    };

    virtual ~Action() = default;

    // progress the simulation forwards one fixed time step
    // action steps run before simulation step
    virtual void step(Entity entity) = 0;

    virtual Status status() const = 0;

    // ask the action to complete or halt
    // can be rejected if doing so would leave the simulation in an invalid state
    virtual Status cancel() = 0;

    // std::string log();

};

struct MoveAction : Action {

    MoveAction(glm::vec2 to);
    MoveAction(float x, float y);
    Status status() const override;
    Status cancel() override;
    void step(Entity entity) override;

private:

    glm::vec2 direction_;

};

/* TODO:
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

