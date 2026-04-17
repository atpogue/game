#include "ui/menu.hh"
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_log.h>

void MainMenu::event(const SDL_Event &e) {
    switch (e.type) {
    case SDL_EVENT_KEY_DOWN:
        if (e.key.scancode != SDL_SCANCODE_ESCAPE
            && e.key.scancode != SDL_SCANCODE_Q) break;
        SDL_Event quit;
        quit.type = SDL_EVENT_QUIT;
        quit.quit.timestamp = SDL_GetTicksNS();
        if (!SDL_PushEvent(&quit))
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not push event: %s\n", SDL_GetError());
        break;
    }
}

void MainMenu::update(f32 dt) {
    time_ += dt;
}

void MainMenu::render(SDL_Renderer *renderer) {
    f32 red   = f32(0.5 + 0.5 * SDL_sin(time_)),
          green = f32(0.5 + 0.5 * SDL_sin(time_ + SDL_PI_D * 2.0 / 3.0)),
          blue  = f32(0.5 + 0.5 * SDL_sin(time_ + SDL_PI_D * 4.0 / 3.0));
    SDL_SetRenderDrawColorFloat(renderer, red, green, blue, SDL_ALPHA_OPAQUE_FLOAT);
    SDL_RenderClear(renderer);
}

