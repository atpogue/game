#pragma once
#include "core/error.hh"
#include "core/types.hh"
#include <glm/ext/vector_int2.hpp>

struct Window;

struct SDL_Window;

[[nodiscard]] Result<Window> create_window(std::string_view title, i32 width, i32 height);

struct Window
{
  Window() noexcept                = default;
  Window(Window const&)            = delete;
  Window& operator=(Window const&) = delete;

  explicit Window(SDL_Window*) noexcept;
  Window(Window&&) noexcept;
  Window& operator=(Window&&) noexcept;
  ~Window() noexcept;

  std::string_view title() const;

  void set_title(std::string_view title);

  [[nodiscard]] glm::ivec2 size() const noexcept;

  void resize(i32 width, i32 height) noexcept;

  [[nodiscard]] bool valid() const noexcept { return handle_ != nullptr; }

  [[nodiscard]] SDL_Window* handle() noexcept { return handle_; }

private:
  SDL_Window* handle_ = nullptr;
};

