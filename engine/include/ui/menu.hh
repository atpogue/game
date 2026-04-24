#pragma once
#include "core/types.hh"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_render.h>

struct Menu {
    virtual ~Menu() = default;
    virtual void event(const SDL_Event &event) = 0;
    virtual void update(f32 dt) = 0;
    virtual void render(SDL_Renderer *renderer) = 0;
};

struct MainMenu : Menu {

    void event(const SDL_Event &e) override;
    void update(f32 dt) override;
    void render(SDL_Renderer *renderer) override;

private:

    f32 time_ = 0.0f;

};

