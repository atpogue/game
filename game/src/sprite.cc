#include "game/sprite.hh"
#include "core/defer.hh"
#include "game/catalog.hh"
#include "game/lua.hh"
#include "game/types.hh"

static Result<Rectangle> try_get_rectangle(lua_State* L, int idx, std::string_view field)
{
  auto rectangle = lua::try_push_field(L, LUA_TTABLE, idx, field);
  if (!rectangle) return std::unexpected(rectangle.error());
  DEFER(lua_pop(L, 1));
  auto x = lua::try_get<f32>(L, *rectangle, 1);
  if (!x) return std::unexpected(x.error());
  auto y = lua::try_get<f32>(L, *rectangle, 2);
  if (!y) return std::unexpected(y.error());
  auto w = lua::try_get<f32>(L, *rectangle, 3);
  if (!w) return std::unexpected(y.error());
  auto h = lua::try_get<f32>(L, *rectangle, 4);
  if (!h) return std::unexpected(y.error());
  return Rectangle({ *x, *y }, { *w, *h });
}

// sprite = { atlas = "path", x = N, y = N, color = N }
Result<Sprite>
lua::try_get_sprite(CatalogWriter catalog, lua_State* L, int idx, std::string_view field)
{
  auto sprite = try_push_field(L, LUA_TTABLE, idx, field);
  if (!sprite) return std::unexpected(sprite.error());
  DEFER(lua_pop(L, 1));
  auto atlas = try_get_string(L, *sprite, "atlas");
  if (!atlas) return std::unexpected(atlas.error());
  auto source = try_get_rectangle(L, *sprite, "source");
  if (!source) return std::unexpected(source.error());
  auto color = try_get<u32>(L, *sprite, "color");
  if (!color) return std::unexpected(color.error());
  auto texture = catalog.find<TextureDef>(*atlas);
  if (!texture) texture = catalog.emplace<TextureDef>(*atlas, TextureDef{ *atlas });
  return Sprite{
    .atlas  = texture,
    .source = *source,
    .tint   = make_color_hex(*color),
  };
}

