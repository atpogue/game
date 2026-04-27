#include "player.hh"
#include "engine/control/command.hh"
#include "engine/control/actor.hh"
#include "engine/ecs/pose.hh"
#include "engine/ecs/ledger.hh"
#include <cassert>

namespace { /////////////////////////////////////////////////////////////////////

    Entity entity;
    PlayerDirector director;

} ///////////////////////////////////////////////////////////////////////////////

Entity player::create(f32 x, f32 y) {
    auto e = ledger::create();
    assert(e && ledger::status(e));
    entity = e;

    auto pose = poses::set(e, { .position = {x, y} });
    assert(pose && poses::get(e));
    assert(ledger::has(e, Flag::Pose));

    actors::create(e, 2);
    assert(ledger::has(e, Flag::Actor));

    return e;
}

Entity player::get() {
    return entity;
}

void player::event(const SDL_Event &e) { director.event(e); }

void player::process_input() {
    for (auto command : director.generate()) {
        actors::act(entity, command);
    }
}

void PlayerDirector::event(const SDL_Event &e) {
    mouse.event(e);
    keyboard.event(e);
}

std::generator<Command> PlayerDirector::generate() {
    f32 x = 0.f, y = 0.f;
    if (keyboard[SDL_SCANCODE_W]) y -= 0.1f;
    if (keyboard[SDL_SCANCODE_S]) y += 0.1f;
    if (keyboard[SDL_SCANCODE_A]) x -= 0.1f;
    if (keyboard[SDL_SCANCODE_D]) x += 0.1f;
    if (x != 0.f || y != 0.f) co_yield make_move_command(next_id++, x, y);

    mouse.flush();
    keyboard.flush();
}

