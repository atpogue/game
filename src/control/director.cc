#include "control/director.hh"
#include "control/command.hh"

void PlayerDirector::event(const SDL_Event &e) {
    mouse_.event(e);
    keyboard_.event(e);
}

std::generator<Command> PlayerDirector::generate() {
    f32 x = 0.f, y = 0.f;
    if (keyboard_[SDL_SCANCODE_W]) y -= 0.1f;
    if (keyboard_[SDL_SCANCODE_S]) y += 0.1f;
    if (keyboard_[SDL_SCANCODE_A]) x -= 0.1f;
    if (keyboard_[SDL_SCANCODE_D]) x += 0.1f;
    if (x != 0.f || y != 0.f) co_yield move_command(x, y);

    mouse_.flush();
    keyboard_.flush();
}

