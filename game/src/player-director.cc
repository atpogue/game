#include "player-director.hh"
#include "control/command.hh"

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

