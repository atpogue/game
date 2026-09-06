#include "game/load.hh"
#include "core/result.hh"
#include "game/catalog.hh"
#include "game/context.hh"
#include "game/terrain.hh"
#include "sdk/node.hh"
#include "sdk/state.hh"
#include <SDL3/SDL_log.h>
#include <string>
#include <utility>
#include <variant>
#include <vector>

namespace {

Status compile_terrain(LuaNode const& table, CatalogWriter catalog)
{
  Result<std::vector<LuaKey>> keys = table.keys();
  if (!keys) return Error(std::move(keys).error());

  for (LuaKey const& key : *keys) {
    std::string const* label = std::get_if<std::string>(&key);
    if (label == nullptr) {
      return Error(describe(table) + ": terrain definition labels must be strings");
    }

    Result<LuaNode> definition = table.find(key);
    if (!definition) return Error(std::move(definition).error());

    Result<Terrain> terrain = parse_terrain(*definition, catalog);
    if (!terrain) return Error(std::move(terrain).error());
    catalog.emplace<Terrain>(*label, std::move(*terrain));
  }
  return {};
}

Status compile_content(LuaState const& state, LoadContext context)
{
  Result<LuaNode> content = state.global("content");
  if (!content) return Error(std::move(content).error());

  Result<LuaNode> terrain = content->find("terrain");
  if (!terrain) return Error(std::move(terrain).error());
  return compile_terrain(*terrain, access_catalog(context));
}

} // namespace

bool load_content(LoadContext context, std::string_view path)
{
  Result<LuaState> state = LuaState::create();
  if (!state) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not initialize Lua: %s", state.error().c_str());
    return false;
  }

  Status result = state->create_table({ "content", "terrain" });
  if (result) result = state->do_file(path);
  // TODO: Normalize metatable-backed content after all content packages have executed.
  if (result) result = compile_content(*state, context);
  if (!result) {
    std::string const terminated_path(path);
    SDL_LogError(
      SDL_LOG_CATEGORY_ERROR, "Error in %s:\n\t%s", terminated_path.c_str(),
      result.error().c_str());
    return false;
  }
  return true;
}
