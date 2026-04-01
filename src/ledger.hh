#pragma once
#include <bitset>
#include <cstdint>
#include <functional>
#include <vector>

enum class Component : uint8_t { Actor, Transform, Max };

constexpr size_t component_index(Component c) { return static_cast<size_t>(c); }

using Entity = uint32_t;
using Signature = std::bitset<component_index(Component::Max)>;

constexpr Entity NULL_ENTITY = 0;

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

