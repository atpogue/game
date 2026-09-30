#pragma once
#include "app/lua/table.hh"
#include <optional>
#include <string>
#include <string_view>

struct lua_State;

namespace Lua {

  // Owns a sandboxed Lua state: only the base, table, string, math, and utf8 libraries are opened.
  // Failures print a diagnostic.
  struct State
  {
    [[nodiscard]] static std::optional<State> create();

    State() noexcept               = default;
    State(State const&)            = delete;
    State& operator=(State const&) = delete;

    State(State&&) noexcept;
    State& operator=(State&&) noexcept;
    ~State() noexcept;

    // Runs the file at the given path.
    [[nodiscard]] bool load(std::string_view path);

    // Runs the source code, using the name to identify the chunk in diagnostics.
    [[nodiscard]] bool execute(std::string_view name, std::string_view source);

    [[nodiscard]] Table globals() const;

    // The path is used to identify the table and its fields in diagnostics.
    [[nodiscard]] Table create_table(std::string path = {}) const;

    [[nodiscard]] lua_State* get() const noexcept { return handle_; }

  private:

    explicit State(lua_State*) noexcept;

    lua_State* handle_ = nullptr;
  };

} // namespace Lua
