#pragma once
#include "core/type-list.hh"
#include "core/types.hh"

enum class Entity : u64 { Nil = UINT64_MAX };

using Components  = TypeList<struct Pose, struct MoveAction>;
using Definitions = TypeList<struct Terrain>;

template <typename Key, typename List>
struct BasicRegistry;

template <typename List>
struct BasicCatalog;

using Catalog = BasicCatalog<Definitions>;

using Registry = BasicRegistry<Entity, Components>;

struct Context;
