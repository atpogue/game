#include "input/keyboard.hh"
#include "input/mouse.hh"
#include "control/director.hh"

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

/* TODO: LLMDirector with planning
struct PendingCommand {
    Command cmd;
    u8 age    = 0u;
    u8 expiry = 0u;
};
*/

