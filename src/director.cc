#include "director.hh"
#include "command.hh"

void PlayerDirector::event(const SDL_Event &e) {
    mouse_.event(e);
    keyboard_.event(e);
}

void PlayerDirector::generate(CommandQueue &queue) {
    float x = 0.f, y = 0.f;
    if (keyboard_[SDL_SCANCODE_W]) y -= 1.f;
    if (keyboard_[SDL_SCANCODE_S]) y += 1.f;
    if (keyboard_[SDL_SCANCODE_A]) x -= 1.f;
    if (keyboard_[SDL_SCANCODE_D]) x += 1.f;
    if (x != 0.f || y != 0.f) queue.push_back(move_command(x, y));

    // translate(keyboard_cmds_, keyboard_, queue);
    // translate(mouse_cmds_, mouse_, keyboard_, queue);

    mouse_.flush();
    keyboard_.flush();
}

