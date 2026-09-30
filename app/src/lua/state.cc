#include "app/lua/state.hh"
#include "core/defer.hh"
#include "core/panic.hh"
#include <lua.hpp>
#include <print>
#include <string>

namespace Lua {

  static int traceback(lua_State* L)
  {
    char const* message = lua_tostring(L, 1);
    if (message == nullptr) return 1;
    luaL_traceback(L, L, message, 1);
    return 1;
  }

  static void open_libraries(lua_State* L)
  {
    struct Library
    {
      char const*   name;
      lua_CFunction open;
    };

    constexpr Library libraries[] = {
      { LUA_GNAME, luaopen_base },        { LUA_TABLIBNAME, luaopen_table },
      { LUA_STRLIBNAME, luaopen_string }, { LUA_MATHLIBNAME, luaopen_math },
      { LUA_UTF8LIBNAME, luaopen_utf8 },
    };

    for (Library const& library : libraries) {
      luaL_requiref(L, library.name, library.open, 1);
      lua_pop(L, 1);
    }

    constexpr char const* unavailable[] = { "collectgarbage", "dofile", "load", "loadfile" };
    for (char const* name : unavailable) {
      lua_pushnil(L);
      lua_setglobal(L, name);
    }
  }

  // Prints and pops the error message on the top of the stack.
  static void report_error(lua_State* L)
  {
    size_t           length  = 0;
    char const*      data    = lua_tolstring(L, -1, &length);
    std::string_view message = data != nullptr ? std::string_view(data, length)
                                               : std::string_view("(error object is not a string)");
    std::println("{}", message);
    lua_pop(L, 1);
  }

  // Calls the function on the top of the stack.
  static bool call(lua_State* L)
  {
    int const function = lua_gettop(L);
    lua_pushcfunction(L, traceback);
    lua_insert(L, function); // place the message handler beneath the function
    DEFER(lua_remove(L, function));
    if (lua_pcall(L, 0, 0, function) == LUA_OK) return true;
    report_error(L);
    return false;
  }

  std::optional<State> State::create()
  {
    lua_State* L = luaL_newstate();
    if (L == nullptr) {
      std::println("failed to create Lua state");
      return std::nullopt;
    }
    open_libraries(L);
    return State(L);
  }

  State::State(lua_State* L) noexcept : handle_{ L } {}

  State::State(State&& other) noexcept : handle_{ other.handle_ } { other.handle_ = nullptr; }

  State& State::operator=(State&& other) noexcept
  {
    if (&other == this) return *this;
    if (handle_ != nullptr) lua_close(handle_);
    handle_       = other.handle_;
    other.handle_ = nullptr;
    return *this;
  }

  State::~State() noexcept
  {
    if (handle_ != nullptr) lua_close(handle_);
  }

  Table State::globals() const
  {
    PRECONDITION(handle_ != nullptr);
    lua_rawgeti(handle_, LUA_REGISTRYINDEX, LUA_RIDX_GLOBALS);
    return Table(Reference::pop(handle_), {});
  }

  Table State::create_table(std::string path) const
  {
    PRECONDITION(handle_ != nullptr);
    lua_newtable(handle_);
    return Table(Reference::pop(handle_), std::move(path));
  }

  bool State::load(std::string_view path)
  {
    PRECONDITION(handle_ != nullptr);
    if (luaL_loadfile(handle_, std::string(path).c_str()) != LUA_OK) {
      report_error(handle_);
      return false;
    }
    return call(handle_);
  }

  bool State::execute(std::string_view name, std::string_view source)
  {
    PRECONDITION(handle_ != nullptr);
    std::string const chunk_name = "=" + std::string(name);
    if (luaL_loadbuffer(handle_, source.data(), source.size(), chunk_name.c_str()) != LUA_OK) {
      report_error(handle_);
      return false;
    }
    return call(handle_);
  }

} // namespace Lua
