#pragma once
#include "input/cue.hh"
#include "input/keyboard.hh"
#include "input/mouse.hh"

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

    /*
    struct {
        KeyboardCue 
            move_up    = {SDL_SCANCODE_W, On::Press | On::Repeat},
            move_down  = {SDL_SCANCODE_S, On::Press | On::Repeat},
            move_left  = {SDL_SCANCODE_A, On::Press | On::Repeat},
            move_right = {SDL_SCANCODE_D, On::Press | On::Repeat};
    } cues_;
    */

    Keyboard keyboard_;
    Mouse mouse_;

    // KeyboardCommandMap keyboard_cmds_;
    // MouseCommandMap mouse_cmds_;

};

// TODO: AIDirector

