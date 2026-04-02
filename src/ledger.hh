#pragma once
#include <bitset>
#include <cstdint>
#include <functional>
#include <vector>

using Entity = uint32_t;
constexpr Entity NULL_ENTITY = 0;

using Id = uint32_t;
constexpr Entity ENTITY_ID_BITS  = 20; // 1,048,575 IDs
constexpr Entity ENTITY_ID_MAX  = (1 << ENTITY_ID_BITS) - 1;
constexpr Id entity_id(Entity e) { return e & ENTITY_ID_MAX; }

constexpr Entity ENTITY_GEN_BITS = 12; // 4,095 generations
constexpr Entity ENTITY_GEN_MAX = (1 << ENTITY_GEN_BITS) - 1;
constexpr uint16_t entity_gen(Entity e) { return (e >> ENTITY_ID_BITS) & ENTITY_GEN_MAX; }

enum class Component : uint8_t { Actor, Transform, Max };
constexpr size_t component_index(Component c) { return static_cast<size_t>(c); }

using Signature = std::bitset<component_index(Component::Max)>;
Signature signature(std::initializer_list<Component> components);

namespace ledger {

    bool status(Entity e);
    Signature signature(Entity e);
    bool has(Entity e, Component c);
    bool has(Entity e, Signature sign);
    bool has(Entity e, std::initializer_list<Component> sign);
    void error(const char *msg);
    const char *error();

    bool sign(Entity e, Component c, bool value);
    Entity create();
    void destroy(Entity e);

    using View = std::vector<Entity>;
    View view(Signature sign);
    View view(std::initializer_list<Component> sign);
    void for_each(Signature sign, std::function<void(Entity)> call);
    void for_each(std::initializer_list<Component> sign, std::function<void(Entity)> call);

} // namespace ledger

