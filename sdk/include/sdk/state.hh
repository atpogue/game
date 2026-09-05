#pragma once
#include "core/result.hh"
#include "sdk/node.hh"
#include "sdk/path.hh"
#include <memory>
#include <string_view>

struct lua_State;

struct LuaState
{
  LuaState()                            = delete;
  LuaState(LuaState const&)             = delete;
  LuaState& operator=(LuaState const&)  = delete;
  LuaState(LuaState&&) noexcept;
  LuaState& operator=(LuaState&&) noexcept;
  ~LuaState() noexcept;

  [[nodiscard]] static Result<LuaState> create();

  [[nodiscard]] Status      create_table(LuaPath const& path);
  [[nodiscard]] Status      do_file(std::string_view path);
  [[nodiscard]] Status      do_string(std::string_view source, std::string_view name = "content");
  [[nodiscard]] Result<LuaNode> global(std::string_view name) const;

private:
  explicit LuaState(std::shared_ptr<lua_State> owner) noexcept;

  [[nodiscard]] Status execute(int load_status);

  std::shared_ptr<lua_State> owner_;
  lua_State* handle_;
};
