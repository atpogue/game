#pragma once
#include "core/types.hh"

// Stable, persistent, cross-session, unique identifier for a simulation object.
enum class Entity : u64 { Nil = UINT64_MAX };

struct Context;
struct ConstContext;
struct LoadContext;

template <AccessPolicy Access>
struct RegistryView;

using RegistryReader = RegistryView<Read>;
using RegistryWriter = RegistryView<Write>;

template <AccessPolicy Access>
struct EntityView;

using EntityReader = EntityView<Read>;
using EntityWriter = EntityView<Write>;

template <AccessPolicy Access>
struct CatalogView;

using CatalogReader = CatalogView<Read>;
using CatalogWriter = CatalogView<Write>;

