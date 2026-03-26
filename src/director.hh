#pragma once
#include "command.hh"
#include "cue.hh"
#include "keyboard.hh"
#include "mouse.hh"

// Generates commands to pass to Actors
struct Director {
    virtual void event(const SDL_Event &e) = 0;
    virtual void generate(CommandQueue &queue) = 0;
    virtual ~Director() = default;
};

struct PlayerDirector : Director {

    void event(const SDL_Event &e) override;
    void generate(CommandQueue &queue) override;

private:

    Keyboard keyboard_;
    KeyboardCommandMap keyboard_cmds_;
    Mouse mouse_;
    MouseCommandMap mouse_cmds_;

};

// TODO: AIDirector

