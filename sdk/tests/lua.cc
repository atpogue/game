#include "sdk/state.hh"
#include "sdk/table.hh"
#include <catch2/catch_test_macros.hpp>
#include <string>
#include <utility>
#include <variant>

namespace {

struct Rectangle
{
  f32 x;
  f32 y;
  f32 width;
  f32 height;
};

struct Sprite
{
  std::string atlas;
  Rectangle   source;
  u32         tint;
};

} // namespace

template <>
struct LuaTraits<Rectangle>
{
  static constexpr LuaField<Rectangle> schema[] = {
    lua_field<&Rectangle::x>(1),
    lua_field<&Rectangle::y>(2),
    lua_field<&Rectangle::width>(3),
    lua_field<&Rectangle::height>(4),
  };
};

template <>
struct LuaTraits<Sprite>
{
  static constexpr LuaField<Sprite> schema[] = {
    lua_field<&Sprite::atlas>("atlas"),
    lua_field<&Sprite::source>("source"),
    lua_field<&Sprite::tint>("tint"),
  };
};

TEST_CASE("Lua nodes resolve typed values through paths")
{
  Result<LuaState> state = LuaState::create();
  REQUIRE(state.has_value());
  REQUIRE(state->do_string("content = { enabled = true, count = 42, name = 'grass' }")
            .has_value());

  Result<LuaNode> content = state->global("content");
  REQUIRE(content.has_value());
  CHECK(content->expect<bool>("enabled") == true);
  CHECK(content->expect<u16>("count") == 42);
  CHECK(content->expect<std::string>("name") == "grass");
}

TEST_CASE("Lua nodes keep their state alive")
{
  Result<LuaNode> content = []() -> Result<LuaNode> {
    Result<LuaState> state = LuaState::create();
    if (!state) return Error(std::move(state).error());
    Status result = state->do_string("content = { value = 42 }");
    if (!result) return Error(std::move(result).error());
    return state->global("content");
  }();

  REQUIRE(content.has_value());
  CHECK(content->expect<i32>("value") == 42);
}

TEST_CASE("Lua schemas recursively translate nested tables")
{
  Result<LuaState> state = LuaState::create();
  REQUIRE(state.has_value());
  REQUIRE(state
            ->do_string(
              "content = { sprite = { atlas = 'tiles.png', source = { 16, 32, 8, 12 }, "
              "tint = 4294967295 } }")
            .has_value());

  Result<LuaNode> content = state->global("content");
  REQUIRE(content.has_value());
  Result<LuaNode> node = content->find("sprite");
  REQUIRE(node.has_value());

  Result<Sprite> sprite = translate<Sprite>(*node);
  REQUIRE(sprite.has_value());
  CHECK(sprite->atlas == "tiles.png");
  CHECK(sprite->source.x == 16.0f);
  CHECK(sprite->source.y == 32.0f);
  CHECK(sprite->source.width == 8.0f);
  CHECK(sprite->source.height == 12.0f);
  CHECK(sprite->tint == 0xffffffffu);
}

TEST_CASE("Lua translation errors identify the complete nested path")
{
  Result<LuaState> state = LuaState::create();
  REQUIRE(state.has_value());
  REQUIRE(state
            ->do_string(
              "content = { sprite = { atlas = 'tiles.png', source = { 16, 32, 'wide', 12 }, "
              "tint = 1 } }")
            .has_value());

  Result<LuaNode> content = state->global("content");
  REQUIRE(content.has_value());
  Result<LuaNode> node = content->find("sprite");
  REQUIRE(node.has_value());

  Result<Sprite> sprite = translate<Sprite>(*node);
  REQUIRE_FALSE(sprite.has_value());
  CHECK(sprite.error() == "content.sprite.source[3]: expected number but found string");
}

TEST_CASE("Lua table keys have deterministic traversal order")
{
  Result<LuaState> state = LuaState::create();
  REQUIRE(state.has_value());
  REQUIRE(state->do_string("content = { z = true, a = true, [2] = true, [1] = true }")
            .has_value());

  Result<LuaNode> content = state->global("content");
  REQUIRE(content.has_value());
  Result<std::vector<LuaKey>> keys = content->keys();
  REQUIRE(keys.has_value());
  REQUIRE(keys->size() == 4);
  CHECK(std::get<i64>((*keys)[0]) == 1);
  CHECK(std::get<i64>((*keys)[1]) == 2);
  CHECK(std::get<std::string>((*keys)[2]) == "a");
  CHECK(std::get<std::string>((*keys)[3]) == "z");
}

TEST_CASE("Lua integer translation rejects narrowing")
{
  Result<LuaState> state = LuaState::create();
  REQUIRE(state.has_value());
  REQUIRE(state->do_string("content = { value = 256 }").has_value());

  Result<LuaNode> content = state->global("content");
  REQUIRE(content.has_value());
  Result<u8> value = content->expect<u8>("value");
  REQUIRE_FALSE(value.has_value());
  CHECK(value.error().contains("outside the destination type's range"));
}
