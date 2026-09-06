#include "game/terrain.hh"
#include "game/catalog.hh"
#include "sdk/node.hh"
#include <utility>

Result<Terrain> parse_terrain(LuaNode const& node, CatalogWriter catalog)
{
  Result<LuaNode> sprite_node = node.find("sprite");
  if (!sprite_node) return Error(std::move(sprite_node).error());

  Result<Sprite> sprite = parse_sprite(*sprite_node, catalog);
  if (!sprite) return Error(std::move(sprite).error());
  return Terrain{ .sprite = std::move(*sprite) };
}
