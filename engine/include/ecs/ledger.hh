#pragma once
#include "core/handle.hh"
#include <bitset>
#include <functional>
#include <vector>

enum class Flag : u8 { Actor, Pose, Sprite, Max };
constexpr size_t flag_index(Flag c) { return static_cast<size_t>(c); }

using Signature = std::bitset<flag_index(Flag::Max)>;
Signature signature(std::initializer_list<Flag> flags);

using Entity = Handle<Signature>;

namespace ledger {

    bool status(Entity e);
    u32 generation(u32 index);
    Signature signature(Entity e);
    bool has(Entity e, Flag f);
    bool has(Entity e, Signature sign);
    bool has(Entity e, std::initializer_list<Flag> sign);
    void error(const char *msg);
    const char *error();

    bool sign(Entity e, Flag c, bool value);
    Entity create();
    void destroy(Entity e);

    using View = std::vector<Entity>;
    View view(Signature sign);
    View view(std::initializer_list<Flag> sign);
    void for_each(Signature sign, std::function<void(Entity)> call);
    void for_each(std::initializer_list<Flag> sign, std::function<void(Entity)> call);

} // namespace ledger

