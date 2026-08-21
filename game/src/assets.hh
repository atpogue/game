#pragma once

// TODO: lazy loading/unloading of assets as needed

#include "core/types.hh"
#include <string>

struct Texture;

struct TextureAsset
{
  std::string     path;
  Handle<Texture> handle;
};

