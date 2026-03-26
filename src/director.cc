#include "director.hh"

void PlayerDirector::event(const SDL_Event &e) {
    mouse_.event(e);
    keyboard_.event(e);
}

void PlayerDirector::generate(CommandQueue &queue) {
    translate(keyboard_cmds_, keyboard_, queue);
    translate(mouse_cmds_, mouse_, keyboard_, queue);
    mouse_.flush();
    keyboard_.flush();
}

