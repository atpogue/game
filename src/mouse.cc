#include "mouse.hh"

bool Mouse::operator[](Uint8 btn) const { return btn >= BUTTON_COUNT ? state_.test(btn) : false; }

bool Mouse::was_pressed(Uint8 btn) const { return btn >= BUTTON_COUNT ? pressed_.test(btn) : false; }

bool Mouse::was_released(Uint8 btn) const { return btn >= BUTTON_COUNT ? released_.test(btn) : false; }

void Mouse::flush() { pressed_.reset(); released_.reset(); }

void Mouse::reset() { pressed_.reset(); released_.reset(); state_.reset(); }

float Mouse::x()  const { return  x_; }
float Mouse::y()  const { return  y_; }
float Mouse::dx() const { return dx_; }
float Mouse::dy() const { return dy_; }
float Mouse::wheel_dx() const { return wdx_; }
float Mouse::wheel_dy() const { return wdy_; }

void Mouse::handle_event(const SDL_Event &event) {
    switch (event.type) {
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
        if (event.button.button >= BUTTON_COUNT) break;
        state_.set(event.button.button, true);
        pressed_.set(event.button.button, true);
        // TODO: event.button.clicks
        break;
    case SDL_EVENT_MOUSE_BUTTON_UP:
        if (event.button.button >= BUTTON_COUNT) break;
        state_.set(event.button.button, false);
        released_.set(event.button.button, true);
        // TODO: event.button.clicks
        break;
    case SDL_EVENT_MOUSE_MOTION:
        dx_ += event.motion.x - x_;
        dy_ += event.motion.y - y_;
        x_ = event.motion.x;
        y_ = event.motion.y;
        break;
    case SDL_EVENT_MOUSE_WHEEL:
        wdx_ = event.wheel.x;
        wdy_ = event.wheel.y;
        break;
    }
}

