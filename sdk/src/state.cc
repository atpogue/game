#include "sdk/state.hh"
#include "core/panic.hh"
#include <format>
#include <lua.hpp>
#include <string>
#include <utility>
#include <variant>

namespace {

void push_key(lua_State* handle, LuaKey const& key)
{
  std::visit(
    [handle](auto const& value) {
      using Type = std::remove_cvref_t<decltype(value)>;
      if constexpr (std::same_as<Type, std::string>) {
        lua_pushlstring(handle, value.data(), value.size());
      } else {
        lua_pushinteger(handle, value);
      }
    },
    key);
}

std::string pop_string(lua_State* handle)
{
  size_t      length = 0;
  char const* data   = lua_tolstring(handle, -1, &length);
  std::string result = data == nullptr ? "unknown Lua error" : std::string(data, length);
  lua_pop(handle, 1);
  return result;
}

int add_traceback(lua_State* handle)
{
  char const* message = lua_tostring(handle, 1);
  if (message == nullptr) return 1;
  luaL_traceback(handle, handle, message, 1);
  return 1;
}

void open_content_libraries(lua_State* handle)
{
  struct Library
  {
    char const*   name;
    lua_CFunction open;
  };

  constexpr Library libraries[] = {
    { LUA_GNAME, luaopen_base },
    { LUA_TABLIBNAME, luaopen_table },
    { LUA_STRLIBNAME, luaopen_string },
    { LUA_MATHLIBNAME, luaopen_math },
    { LUA_UTF8LIBNAME, luaopen_utf8 },
  };

  for (Library const& library : libraries) {
    luaL_requiref(handle, library.name, library.open, 1);
    lua_pop(handle, 1);
  }

  constexpr char const* unavailable[] = { "collectgarbage", "dofile", "load", "loadfile" };
  for (char const* name : unavailable) {
    lua_pushnil(handle);
    lua_setglobal(handle, name);
  }
}

} // namespace

LuaState::LuaState(std::shared_ptr<lua_State> owner) noexcept
  : owner_{ std::move(owner) }, handle_{ owner_.get() }
{
  PRECONDITION(handle_ != nullptr);
}

LuaState::LuaState(LuaState&& other) noexcept
  : owner_{ std::move(other.owner_) }, handle_{ owner_.get() }
{
  other.handle_ = nullptr;
}

LuaState& LuaState::operator=(LuaState&& other) noexcept
{
  if (&other == this) return *this;
  owner_        = std::move(other.owner_);
  handle_       = owner_.get();
  other.handle_ = nullptr;
  return *this;
}

LuaState::~LuaState() noexcept = default;

Result<LuaState> LuaState::create()
{
  lua_State* handle = luaL_newstate();
  if (handle == nullptr) return Error("failed to create Lua state");

  std::shared_ptr<lua_State> owner(handle, lua_close);
  open_content_libraries(handle);
  return LuaState(std::move(owner));
}

Status LuaState::create_table(LuaPath const& path)
{
  PRECONDITION(handle_ != nullptr);
  if (path.empty()) return Error("cannot create a table at an empty Lua path");

  int const base = lua_gettop(handle_);
  lua_rawgeti(handle_, LUA_REGISTRYINDEX, LUA_RIDX_GLOBALS);

  LuaPath current;
  for (LuaKey const& key : path) {
    current += key;
    push_key(handle_, key);
    int const type = lua_rawget(handle_, -2);

    if (type == LUA_TNIL) {
      lua_pop(handle_, 1);
      lua_newtable(handle_);
      push_key(handle_, key);
      lua_pushvalue(handle_, -2);
      lua_rawset(handle_, -4);
    } else if (type != LUA_TTABLE) {
      std::string message = std::format(
        "{}: expected table but found {}", describe(current), lua_typename(handle_, type));
      lua_settop(handle_, base);
      return Error(std::move(message));
    }

    lua_remove(handle_, -2);
  }

  lua_settop(handle_, base);
  return {};
}

Status LuaState::do_file(std::string_view path)
{
  PRECONDITION(handle_ != nullptr);
  std::string const terminated_path(path);
  return execute(luaL_loadfile(handle_, terminated_path.c_str()));
}

Status LuaState::do_string(std::string_view source, std::string_view name)
{
  PRECONDITION(handle_ != nullptr);
  std::string const terminated_name(name);
  return execute(luaL_loadbuffer(handle_, source.data(), source.size(), terminated_name.c_str()));
}

Status LuaState::execute(int load_status)
{
  PRECONDITION(handle_ != nullptr);
  if (load_status != LUA_OK) return Error(pop_string(handle_));

  int const function = lua_gettop(handle_);
  lua_pushcfunction(handle_, add_traceback);
  lua_insert(handle_, function);

  int const call_status = lua_pcall(handle_, 0, 0, function);
  if (call_status != LUA_OK) {
    std::string message = pop_string(handle_);
    lua_remove(handle_, function);
    return Error(std::move(message));
  }

  lua_remove(handle_, function);
  return {};
}

Result<LuaNode> LuaState::global(std::string_view name) const
{
  PRECONDITION(handle_ != nullptr);
  lua_rawgeti(handle_, LUA_REGISTRYINDEX, LUA_RIDX_GLOBALS);
  lua_pushlstring(handle_, name.data(), name.size());
  int const type = lua_rawget(handle_, -2);
  lua_remove(handle_, -2);

  if (type == LUA_TNIL) {
    lua_pop(handle_, 1);
    return Error(std::format("{}: no value found", name));
  }

  i32 const reference = luaL_ref(handle_, LUA_REGISTRYINDEX);
  return LuaNode(owner_, reference, LuaPath{ LuaKey{ std::string(name) } });
}
