#include "game/sprite.hh"
#include "game/catalog.hh"
#include "game/texture.hh"
#include "sdk/table.hh"
#include <string>
#include <utility>

namespace {

struct RectangleData
{
  f32 x;
  f32 y;
  f32 width;
  f32 height;
};

struct SpriteData
{
  std::string   atlas;
  RectangleData source;
  u32           color;
};

} // namespace

template <>
struct LuaTraits<RectangleData>
{
  static constexpr LuaField<RectangleData> schema[] = {
    lua_field<&RectangleData::x>(1),
    lua_field<&RectangleData::y>(2),
    lua_field<&RectangleData::width>(3),
    lua_field<&RectangleData::height>(4),
  };
};

template <>
struct LuaTraits<SpriteData>
{
  static constexpr LuaField<SpriteData> schema[] = {
    lua_field<&SpriteData::atlas>("atlas"),
    lua_field<&SpriteData::source>("source"),
    lua_field<&SpriteData::color>("color"),
  };
};

Result<Sprite> parse_sprite(LuaNode const& node, CatalogWriter catalog)
{
  Result<SpriteData> data = translate<SpriteData>(node);
  if (!data) return Error(std::move(data).error());

  Token<TextureDef> texture = catalog.find<TextureDef>(data->atlas);
  if (!texture) {
    texture = catalog.emplace<TextureDef>(data->atlas, TextureDef{ data->atlas });
  }

  return Sprite{
    .atlas = texture,
    .source = Rectangle{
      .origin = { data->source.x, data->source.y },
      .extent = { data->source.width, data->source.height },
    },
    .tint = make_color_hex(data->color),
  };
}
