#include "sys/window.hh"
#include "core/panic.hh"
#include <SDL3/SDL_video.h>
#include <glm/ext/vector_int2.hpp>
#include <string>

Window::Window(SDL_Window* handle) noexcept : handle_{ handle } { PRECONDITION(handle != nullptr); }

Window::Window(Window&& other) noexcept : handle_{ other.handle_ } { other.handle_ = nullptr; }

Window& Window::operator=(Window&& other) noexcept
{
  if (&other == this) return *this;
  handle_       = other.handle_;
  other.handle_ = nullptr;
  return *this;
}

Window::~Window() noexcept { SDL_DestroyWindow(handle_); }

Result<Window> create_window(std::string_view title, i32 width, i32 height)
{
  ASSERT(width != 0 && height != 0);
  auto window = SDL_CreateWindow(std::string(title).c_str(), width, height, 0);
  if (!window) return std::unexpected<Error>(SDL_GetError());
  return Window(window);
}

std::string_view Window::title() const { return SDL_GetWindowTitle(handle_); }

void Window::set_title(std::string_view title)
{
  SDL_SetWindowTitle(handle_, std::string(title).c_str());
}

glm::ivec2 Window::size() const noexcept
{
  i32 w, h;
  SDL_GetWindowSizeInPixels(handle_, &w, &h);
  return { w, h };
}

void Window::resize(i32 width, i32 height) noexcept { SDL_SetWindowSize(handle_, width, height); }

