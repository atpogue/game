#include "app/lua/state.hh"
#include "app/lua/table.hh"
#include "app/lua/value.hh"
#include <catch2/catch_test_macros.hpp>
#include <optional>
#include <string>
#include <utility>

namespace {
  struct Point
  {
    f32 x;
    f32 y;
  };
}

namespace Lua {
  static bool read(Point& dst, Table const& src)
  {
    bool ok  = true;
    ok      &= src["x"].read(dst.x);
    ok      &= src["y"].read(dst.y);
    return ok;
  }
}

static Lua::State make_state(std::string_view source)
{
  std::optional<Lua::State> state = Lua::State::create();
  REQUIRE(state.has_value());
  REQUIRE(state->execute("test", source));
  return std::move(*state);
}

TEST_CASE("Lua values read scalar fields")
{
  Lua::State const state = make_state("content = { enabled = true, count = 42, name = 'grass' }");

  std::optional<Lua::Table> content = state.globals()["content"].expect_table();
  REQUIRE(content.has_value());

  bool        enabled = false;
  u16         count   = 0;
  std::string name;
  CHECK((*content)["enabled"].read(enabled));
  CHECK((*content)["count"].read(count));
  CHECK((*content)["name"].read(name));
  CHECK(enabled == true);
  CHECK(count == 42);
  CHECK(name == "grass");
}

TEST_CASE("Lua values track their path")
{
  Lua::State const state = make_state("content = { sprite = { source = { 16, 32, 'wide', 12 } } }");

  std::optional<Lua::Table> content = state.globals()["content"].expect_table();
  REQUIRE(content.has_value());
  std::optional<Lua::Table> sprite = (*content)["sprite"].expect_table();
  REQUIRE(sprite.has_value());
  std::optional<Lua::Table> source = (*sprite)["source"].expect_table();
  REQUIRE(source.has_value());
  CHECK(source->path() == "content.sprite.source");

  Lua::Value const width = (*source)[3];
  CHECK(width.path() == "content.sprite.source[3]");
  CHECK(width.is_string());

  f32 number = 0.f;
  CHECK_FALSE(width.read(number));

  Lua::Table const terrain = state.create_table("content.terrain");
  CHECK(terrain["grass-1"].path() == R"(content.terrain["grass-1"])");
}

TEST_CASE("Lua numbers accept integers")
{
  Lua::State const state = make_state("content = { integer = 16, float = 2.0, fraction = 2.5 }");

  std::optional<Lua::Table> content = state.globals()["content"].expect_table();
  REQUIRE(content.has_value());

  f32 number = 0.f;
  CHECK((*content)["integer"].read(number));
  CHECK(number == 16.f);

  i32 integer = 0;
  CHECK((*content)["float"].read(integer));
  CHECK(integer == 2);
  CHECK_FALSE((*content)["fraction"].read(integer));
}

TEST_CASE("Lua integer reads reject narrowing")
{
  Lua::State const state = make_state("content = { value = 256, negative = -1 }");

  std::optional<Lua::Table> content = state.globals()["content"].expect_table();
  REQUIRE(content.has_value());

  u8 small = 0;
  CHECK_FALSE((*content)["value"].read(small));
  CHECK_FALSE((*content)["negative"].read(small));

  u64 large = 0;
  CHECK_FALSE((*content)["negative"].read(large));
  CHECK((*content)["value"].read(large));
  CHECK(large == 256u);
}

TEST_CASE("Lua tables read user-defined types")
{
  Lua::State const state = make_state("point = { x = 1.5, y = -2 }");

  std::optional<Lua::Table> table = state.globals()["point"].expect_table();
  REQUIRE(table.has_value());

  Point point{};
  CHECK(table->read(point));
  CHECK(point.x == 1.5f);
  CHECK(point.y == -2.f);
}

TEST_CASE("Lua table keys have deterministic traversal order")
{
  Lua::State const state = make_state("content = { z = true, a = true, [2] = true, [1] = true }");

  std::optional<Lua::Table> content = state.globals()["content"].expect_table();
  REQUIRE(content.has_value());

  std::vector<Lua::Key> const keys = content->keys();
  REQUIRE(keys.size() == 4);
  CHECK(keys[0].as_index() == 1);
  CHECK(keys[1].as_index() == 2);
  CHECK(keys[2].as_name() == "a");
  CHECK(keys[3].as_name() == "z");
}

TEST_CASE("Lua tables can be modified from C++")
{
  std::optional<Lua::State> state = Lua::State::create();
  REQUIRE(state.has_value());

  Lua::Table const globals = state->globals();
  Lua::Table const content = state->create_table();
  content.set("flag", true);
  content.set("count", 3);
  content.set("ratio", 0.5);
  content.set("name", "grass");
  content.set(1, content);
  globals.set("content", content);

  constexpr std::string_view script = R"(
    assert(content.flag == true and content.count == 3 and content.ratio == 0.5)
    assert(content.name == 'grass' and content[1] == content)
    content.flag = nil
  )";
  REQUIRE(state->execute("test", script));

  CHECK(content["flag"].is_nil());
  CHECK(content["count"].is_integer());
  CHECK(content["ratio"].is_number());
  CHECK(content[1].reference() == content.reference());

  content.erase("name");
  CHECK(content["name"].is_nil());
}

TEST_CASE("Lua references survive their parent and a moved state")
{
  std::optional<Lua::State> state = Lua::State::create();
  REQUIRE(state.has_value());
  REQUIRE(state->execute("test", "content = { name = 'grass' }"));

  std::optional<Lua::Value> name;
  {
    std::optional<Lua::Table> content = state->globals()["content"].expect_table();
    REQUIRE(content.has_value());
    name = (*content)["name"];
  }
  REQUIRE(state->execute("test", "content = nil"));

  Lua::State const moved = std::move(*state);
  REQUIRE(name->is_string());
  CHECK(name->to_string() == "grass");

  Lua::Value const copy = *name;
  CHECK(copy.reference() == name->reference());
  name.reset();
  CHECK(copy.to_string() == "grass");
}

TEST_CASE("Lua scripts report errors")
{
  std::optional<Lua::State> state = Lua::State::create();
  REQUIRE(state.has_value());
  CHECK_FALSE(state->execute("syntax", "content = {"));
  CHECK_FALSE(state->execute("runtime", "error('failure')"));
  CHECK_FALSE(state->execute("sandbox", "dofile('content/terrain.lua')"));
  CHECK_FALSE(state->load("missing.lua"));
}
