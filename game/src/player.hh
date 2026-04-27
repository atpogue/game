#include "engine/control/director.hh"
#include "engine/ecs/ledger.hh"
#include "engine/input/keyboard.hh"
#include "engine/input/mouse.hh"

namespace player {
    Entity create(f32 x, f32 y);
    Entity get();
    void event(const SDL_Event &e);
    void process_input();
}

struct PlayerDirector : Director {

    void event(const SDL_Event &e) override;
    std::generator<Command> generate() override;

private:

    /*
    struct {
        KeyboardCue 
            move_up    = {SDL_SCANCODE_W, With::None, On::Press | On::Repeat},
            move_down  = {SDL_SCANCODE_S, With::None, On::Press | On::Repeat},
            move_left  = {SDL_SCANCODE_A, With::None, On::Press | On::Repeat},
            move_right = {SDL_SCANCODE_D, With::None, On::Press | On::Repeat};
    } cues;
    */

    Keyboard keyboard;
    Mouse mouse;
    u32 next_id = 0u;

};

