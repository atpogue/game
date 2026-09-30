#pragma once
#include "app/sprite.hh"
#include "core/basic-catalog.hh"
#include "gfx/texture.hh"
#include <string_view>
#include <vector>

struct Renderer;

namespace Game { struct World; }

namespace App {
  using Assets  = TypeList<Texture>;
  using Catalog = BasicCatalog<Assets>; // labeled by file path

  // The presentation of content definitions.
  struct Visuals
  {
    Catalog             assets;
    std::vector<Sprite> terrain; // parallel to the world's terrain definitions
  };

  // Runs the Lua content script at the path, then compiles the definitions it authored into the
  // world's catalog and their presentation into the visuals. Diagnostics are printed to stdout.
  // Returns false if the script failed or any definition could not be compiled.
  [[nodiscard]] bool
  load_content(std::string_view path, Game::World& world, Renderer& renderer, Visuals& visuals);
}
