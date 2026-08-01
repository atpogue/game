#include "core/basic-registry.hh"
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <set>

namespace { //////////////////////////////////////////////////////////////////////////////

  struct Position
  {
    float x = 0.f, y = 0.f;
  };

  struct Velocity
  {
    float dx = 0.f, dy = 0.f;
  };

  struct Health
  {
    int hp = 100;
  };

  struct Counted
  {
    static inline int instances = 0;
    int               value;

    explicit Counted(int v = 0) : value{v} { ++instances; }

    Counted(Counted const& o) : value{o.value} { ++instances; }

    Counted& operator=(Counted const& o) = default;

    ~Counted() { --instances; }
  };

  struct Entity
  {
    std::string name;
  };

  using Components = TypeList<Position, Velocity, Health, Counted>;
} // namespace

TEST_CASE("Registry – entity creation and destruction", "[registry][entity]")
{
  BasicRegistry<Entity, Components> r;

  SECTION("creating entities")
  {
    std::set<Handle<Entity>> entities;
    for (u32 i = 0; i < 10; ++i) {
      auto [h, is_unique] = entities.emplace(r.create());
      REQUIRE(is_unique);
      REQUIRE(*h != Handle<Entity>::null());
      REQUIRE(r.size() == i + 1u);
      REQUIRE(r.valid(*h));
    }
  }

  SECTION("destroying entities")
  {
    std::array<Handle<Entity>, 10> entities;
    for (auto& h : entities) h = r.create();
    for (u32 i = 0u; i < 10u; ++i) {
      REQUIRE(r.size() == 10u - i);
      auto const h = entities[i];
      r.destroy(h);
      REQUIRE(r.valid(h) == false);
    }
    REQUIRE(r.size() == 0u);
  }

  SECTION("polling non-existent entity")
  {
    auto h = GENERATE(Handle<Entity>{}, Handle<Entity>{48u, 29u});
    REQUIRE(r.valid(h) == false);
    REQUIRE(r.has<Position>(h) == false);
    REQUIRE(r.try_get<Position>(h) == nullptr);
  }

  SECTION("polling non-existent component")
  {
    auto h = r.create();
    GENERATE(1, 2, 3);
    REQUIRE(r.has<Position>(h) == false);
    REQUIRE(r.try_get<Position>(h) == nullptr);
  }

  SECTION("emplacing components")
  {
    auto h = r.create();

    // emplace constructs the component type in-place
    auto& ref = r.emplace<Counted>(h, 381);
    REQUIRE(ref.value == 381);
    REQUIRE(Counted::instances == 1u);

    // entity should have ownership of the component
    REQUIRE(r.has<Counted>(h));

    // the address of the owned component should match the one constructed
    Counted* ptr = r.try_get<Counted>(h);
    REQUIRE(ptr != nullptr);
    REQUIRE(ptr == &ref);
  }

  SECTION("mutating components")
  {
    auto h = r.create();

    // emplace returns an mutable reference to constructed component
    auto& p = r.emplace<Position>(h, 0.f, 0.f);
    p.x     = 99.f;
    REQUIRE(r.get<Position>(h).x == 99.f);

    // get returns an mutable reference to existing component
    r.get<Position>(h).y = 381.f;
    REQUIRE(r.get<Position>(h).y == 381.f);
  }

  SECTION("erasing component")
  {
    auto h = r.create();
    r.emplace<Counted>(h);
    r.emplace<Health>(h);

    r.erase<Counted>(h);
    REQUIRE(r.has<Counted>(h) == false);
    REQUIRE(r.try_get<Counted>(h) == nullptr);

    // destructor should have been called
    REQUIRE(Counted::instances == 0u);

    // erasing one component should not affect other components
    REQUIRE(r.has<Health>(h));

    // component may be emplaced again after erase
    r.emplace<Counted>(h);
    REQUIRE(r.has<Counted>(h));
    REQUIRE(r.try_get<Counted>(h) != nullptr);
    REQUIRE(Counted::instances == 1u);
  }

  SECTION("component lifetime")
  {
    auto h = r.create();
    r.emplace<Position>(h);
    r.emplace<Velocity>(h);
    r.emplace<Health>(h);
    r.destroy(h);

    // components should be tied to entity lifetime
    REQUIRE(r.has<Position>(h) == false);
    REQUIRE(r.has<Velocity>(h) == false);
    REQUIRE(r.has<Health>(h) == false);
    REQUIRE(r.try_get<Position>(h) == nullptr);
    REQUIRE(r.try_get<Velocity>(h) == nullptr);
    REQUIRE(r.try_get<Health>(h) == nullptr);
  }

  SECTION("all")
  {
    Handle<Entity> h = r.create();
    REQUIRE_FALSE(r.all<Position, Velocity>(h));
    r.emplace<Position>(h);
    REQUIRE_FALSE(r.all<Position, Velocity>(h));
    r.emplace<Velocity>(h);
    REQUIRE(r.all<Position, Velocity>(h));
  }

  SECTION("any")
  {
    Handle<Entity> h = r.create();
    REQUIRE_FALSE(r.any<Position, Velocity>(h));
    r.emplace<Position>(h);
    REQUIRE(r.any<Position, Velocity>(h));
    r.emplace<Velocity>(h);
    REQUIRE(r.any<Position, Velocity>(h));
  }

  SECTION("iterating")
  {
    // iterating empty registry
    int count = 0;
    for (auto [h, _] : r) ++count;
    REQUIRE(count == 0);

    Handle<Entity> a = r.create(), b = r.create(), c = r.create(), d = r.create();
    bool           saw_a = false, saw_b = false, saw_c = false, saw_d = false;

    // all entities are present in iteration
    for (auto [e, _] : r) {
      ++count;
      if (e == a) saw_a = true;
      if (e == b) saw_b = true;
      if (e == c) saw_c = true;
      if (e == d) saw_d = true;
    }
    REQUIRE(count == 4);
    REQUIRE(saw_a);
    REQUIRE(saw_b);
    REQUIRE(saw_c);

    // destroyed entities are absent in iteration
    saw_b = false;
    saw_d = false;
    count = 0;
    r.destroy(b);
    r.destroy(d);
    for (auto [e, _] : r) {
      ++count;
      if (e == b) saw_b = true;
      if (e == d) saw_d = true;
    }
    REQUIRE(count == 2);
    REQUIRE_FALSE(saw_b);
    REQUIRE_FALSE(saw_d);
  }

  SECTION("clear")
  {
    r.emplace<Counted>(r.create());
    r.emplace<Counted>(r.create());

    r.clear();
    REQUIRE(r.size() == 0u);
    REQUIRE(Counted::instances == 0u);

    // registry is usable after clear
    r.emplace<Counted>(r.create());
    REQUIRE(Counted::instances == 1u);
    REQUIRE(r.size() == 1u);
  }

  SECTION("entity meta-data")
  {
    Handle<Entity> h = r.create("foo");
    for (auto [_, e] : r) REQUIRE(e.name == "foo");
    // meta-data is independent, per-entity
    (void)r.create("bar");
    Entity const& e = r[h];
    REQUIRE(e.name == "foo");
  }
}
