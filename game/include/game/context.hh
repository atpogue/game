#pragma once

// Contexts are an attempt to constrain the simulations API to only the methods the caller needs to
// fulfill their function. For example, simulation systems should be able to read but not write to
// the catalog (which is supposed to be immutable after load), and the UI should be able to read but
// not write to simulation state.

#include "game/types.hh"

struct Tile;
struct Simulation;

[[nodiscard]] Entity who(ConstContext ctx, Handle<Entity> handle);

[[nodiscard]] Handle<Entity> find_entity(ConstContext ctx, Entity id);

[[nodiscard]] bool is_valid(ConstContext ctx, Handle<Entity> handle);

struct ConstContext
{
  ConstContext(ConstContext const&) noexcept = default;
  ConstContext(ConstContext&&) noexcept      = default;
  ~ConstContext() noexcept                   = default;

private:

  friend struct Simulation;
  friend struct Context;
  friend struct LoadContext;
  friend RegistryReader access_registry(ConstContext);
  friend CatalogReader  access_catalog(ConstContext);
  friend EntityReader   access_entity(ConstContext, Handle<Entity>);
  friend Handle<Entity> find_entity(ConstContext, Entity);
  friend Entity         who(ConstContext, Handle<Entity>);
  friend bool           is_valid(ConstContext, Handle<Entity>);
  friend Tile const*    tile_at(ConstContext ctx, u32 x, u32 y);

  ConstContext(Simulation const& src) noexcept : src_{ src } {}

  Simulation const* operator->() const { return &src_; }

  Simulation const& operator*() const { return src_; }

  Simulation const& src_;
};

struct Context
{
  Context(Context const&) noexcept = default;
  Context(Context&&) noexcept      = default;
  ~Context() noexcept              = default;

  operator ConstContext() const noexcept { return ConstContext(src_); }

private:

  friend struct Simulation;
  friend struct LoadContext;
  friend RegistryWriter access_registry(Context);
  friend EntityWriter   access_entity(Context, Handle<Entity>);
  friend EntityWriter   create_entity(Context ctx);

  Context(Simulation& src) noexcept : src_{ src } {}

  Simulation* operator->() const { return &src_; }

  Simulation& operator*() const { return src_; }

  Simulation& src_;
};

struct LoadContext
{
  LoadContext(LoadContext const&) noexcept = default;
  LoadContext(LoadContext&&) noexcept      = default;
  ~LoadContext() noexcept                  = default;

  operator ConstContext() const noexcept { return ConstContext(src_); }

  operator Context() const noexcept { return Context(src_); }

private:

  friend struct Simulation;
  friend CatalogWriter access_catalog(LoadContext);

  LoadContext(Simulation& src) noexcept : src_{ src } {}

  Simulation* operator->() const { return &src_; }

  Simulation& operator*() const { return src_; }

  Simulation& src_;
};

