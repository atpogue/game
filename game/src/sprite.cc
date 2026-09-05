#include "game/catalog.hh"
#include "game/context.hh"
#include "game/sprite.hh"
#include "sdk/node.hh"

Result<Rectangle> parse_rectangle(LuaNode const& node)
{
  Rectangle rectangle;

  if (auto x = node.expect_number(1)) {
    rectangle.origin.x = f32(*x);
  } else return err(x);

  if (auto y = node.expect_number(2)) {
    rectangle.origin.y = f32(*y);
  } else return err(y);

  if (auto w = node.expect_number(3)) {
    rectangle.extent.x = f32(*w);
  } else return err(w);

  if (auto h = node.expect_number(4)) {
    rectangle.extent.y = f32(*h);
  } else return err(h);

  return rectangle;
}

Result<Sprite> parse_sprite(LuaNode const& node)
{
  auto   catalog = access_catalog(node.context());
  Sprite sprite;

  if (auto atlas = node.expect_string("atlas")) {
    auto texture = catalog.find<TextureDef>(*atlas);
    if (!texture) texture = catalog.emplace<TextureDef>(*atlas, TextureDef{ *atlas });
    sprite.atlas = texture;
  } else return err(atlas);

  if (auto source = node.find("source").and_then(parse_rectangle)) {
    sprite.source = *source;
  } else return err(source);

  if (auto color = node.expect_integer("color")) {
    sprite.tint = make_color_hex(*color);
  } else return err(color);

  return sprite;
}

