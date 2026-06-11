#pragma once
#include <expected>
#include <lua.hpp>
#include <string>
#include <format>

// helper functions for Lua's C API
namespace lua {

    struct Error {
        enum class Type {
            Runtime = LUA_ERRRUN,
            Syntax = LUA_ERRSYNTAX,
            Memory = LUA_ERRMEM,
            ErrorHandler = LUA_ERRERR,
            File = LUA_ERRFILE,
            Content = 100,
        } type;
        std::string msg;
    };

    [[nodiscard]] Error make_content_error(lua_State *L, int found, int expected);

    template <typename Type>
    using Result = std::expected<Type, Error>;

    void add_global_function(lua_State *L, const char *name, lua_CFunction fn);
    void add_global_userdata(lua_State *L, const char *name, void *ptr);

    void push_string(lua_State *L, std::string_view value);

    template <typename... Args>
    void push_fstring(lua_State *L, std::format_string<Args...> fmt, Args &&... args) {
        auto str = std::format(fmt, std::forward<Args>(args)...);
        lua_pushstring(L, str.data());
    }

    [[nodiscard]] std::string pop_string(lua_State *L);
    [[nodiscard]] lua_Integer pop_integer(lua_State *L);
    [[nodiscard]] lua_Number pop_number(lua_State *L);
    [[nodiscard]] bool pop_boolean(lua_State *L);

    // Only pushes onto the stack if the expected type is found.
    // Returns: absolute index of the new item, or a content error if the expected type wasn't found.
    // Assumes: [L] is not null, [table] is non-zero, value at [table] is a table, [field] is non-zero.
    [[nodiscard]] Result<int> try_push_field(lua_State *L, int type, int table, unsigned field);

    // Only pushes onto the stack if the expected type is found.
    // Returns: absolute index of the new item, or a content error if the expected type wasn't found.
    // Assumes: [L] is not null, [table] is non-zero, value at [table] is a table
    [[nodiscard]] Result<int> try_push_field(lua_State *L, int type, int table, std::string_view field);

    [[nodiscard]] Result<bool> try_get_boolean(lua_State *L, int table, unsigned field);
    [[nodiscard]] Result<bool> try_get_boolean(lua_State *L, int table, std::string_view field);

    [[nodiscard]] Result<std::string> try_get_string(lua_State *L, int table, unsigned field);
    [[nodiscard]] Result<std::string> try_get_string(lua_State *L, int table, std::string_view field);

    [[nodiscard]] Result<lua_Integer> try_get_integer(lua_State *L, int table, unsigned field);
    [[nodiscard]] Result<lua_Integer> try_get_integer(lua_State *L, int table, std::string_view field);

    [[nodiscard]] Result<lua_Number> try_get_number(lua_State *L, int table, unsigned field);
    [[nodiscard]] Result<lua_Number> try_get_number(lua_State *L, int table, std::string_view field);

    [[nodiscard]] Result<void> do_file(lua_State *L, std::string_view path, unsigned argc=0u, unsigned resultc=0u);

} // namespace lua

