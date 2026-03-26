#include "menu.hh"
#include <cstdlib>
#include <cmath>
#include <SDL3/SDL.h>
#include <iostream>

struct {
    MainMenu main_menu;
} ui;

SDL_Window *window = nullptr;
SDL_Renderer *renderer = nullptr;

// The length of a fixed time step in seconds
const float step_size = 1.0f / 60.0f;

// Get the number of seconds elapsed since SDL was initialized
float time() { return float(SDL_GetTicks()) / 1000.0f; }

bool open() {
    window = SDL_CreateWindow("Hello World", 800, 400, 0);
    if (!window) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not create window: %s\n", SDL_GetError());
        return false;
    }

    renderer = SDL_CreateRenderer(window, NULL);
    if (!renderer) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not create renderer: %s\n", SDL_GetError());
        return false;
    }

    return true;
}

bool close() {
    SDL_DestroyWindow(window);
    SDL_Quit();
    return true;
}

void render() {
    SDL_RenderClear(renderer);
    //SDL_SetRenderDrawColor(renderer, 255u, 160u, 16u, 0u);
    ui.main_menu.render(renderer);
    SDL_RenderPresent(renderer);
}

void update(float dt) {
    ui.main_menu.update(dt);
}

// progress the simulation forwards one fixed time step
void step() {
}

// process all window events and other high priority events 
bool process_event(const SDL_Event &e) {
    bool play = true;
    switch (e.type) {
    case SDL_EVENT_QUIT:
    case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
        play = false;
        break;
    case SDL_EVENT_WINDOW_FOCUS_LOST:
        SDL_ResetKeyboard();
        break;
    }
    return play;
}

// send events to interested systems
void dispatch_event(const SDL_Event &e) {
    ui.main_menu.event(e);
}

int main(void) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "Could not initialize SDL: " << SDL_GetError() << '\n';
        return EXIT_SUCCESS;
    }

    bool play = open();
    float lag = 0.0f,
          prior = time(); // time of start of last frame in seconds

    while (play) {
        float t = time(), // time of start of current frame in seconds
              dt = t - prior; // time that elapsed since the start of last frame

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            play = process_event(event);
            dispatch_event(event);
        }

        update(fmin(dt, 0.25f));
        for (lag += dt; lag >= step_size; lag -= step_size) {
            step();
        }

        render();
        prior = t;
    }

    if (!close()) return EXIT_FAILURE;
    return EXIT_SUCCESS;
}

