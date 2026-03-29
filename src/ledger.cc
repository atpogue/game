#include "ledger.hh"
#include <cassert>
#include <vector>
#include <queue>

constexpr Entity ENTITY_ID_BITS  = 20; // 1,048,575 IDs
constexpr Entity ENTITY_GEN_BITS = 12; // 4,095 generations
constexpr Entity ENTITY_ID_MASK  = (1 << ENTITY_ID_BITS) - 1;
constexpr Entity ENTITY_GEN_MASK = (1 << ENTITY_GEN_BITS) - 1;

constexpr uint32_t id(Entity e) { return e & ENTITY_ID_MASK; }
constexpr uint16_t gen(Entity e) { return (e >> ENTITY_ID_BITS) & ENTITY_GEN_MASK; }

namespace {
    struct EntityData {
        bool live;
        uint16_t generation;
        Signature sign;
        Index index;
    };

    std::vector<EntityData> entities(1, EntityData{false,0});
    std::queue<Entity> dead;
}

bool ledger::status(Entity e) {
    auto i = id(e);
    if (i < entities.size()) return false;
    return entities[i].live && entities[i].generation == gen(e);
}

bool ledger::has(Entity e, Component c) {
    return ledger::status(e) ? entities[id(e)].sign.test(c) : false;
}
// is this okay? it would mean that out-of-bounds entity would fail silently, not indicate failure, but it also means that
// a value is returned, not pointer to value, the validity of e can be checked via `ledger::status(e)`
// It's like a "you give trash, you get trash" agreement.
Signature ledger::signature(Entity e) {
    return ledger::status(e) ? entities[id(e)].sign : entities[NULL_ENTITY].sign;
}

Index ledger::index(Entity e) {
    return ledger::status(e) ? entities[id(e)].index : entities[NULL_ENTITY].index;
}

size_t ledger::index(Entity e, Component c) {
    return ledger::status(e) ? entities[id(e)].index[c] : entities[NULL_ENTITY].index[c];
}

bool ledger::assign(Entity e, Component c, size_t i) {
    if (!ledger::status(e)) return false;
    entities[id(e)].sign.set(c, true);
    entities[id(e)].index[c] = i;
    return true;
}

Entity ledger::create() {
    Entity e = NULL_ENTITY;
    if (!dead.empty()) {
        auto i = id(dead.front());
        dead.pop();
        uint16_t g = entities[i].generation + 1;
        assert(g < ENTITY_GEN_MASK);
        entities[i] = EntityData{true, g};
        e = (g << ENTITY_ID_BITS) | i;
    } else if (entities.size() < ENTITY_ID_MASK) {
        auto i = entities.size();
        entities.emplace_back(true, 0);
        e = i;
    }
    return e;
}

void ledger::destroy(Entity e) {
    if (!ledger::status(e)) return;
    auto i = id(e);
    if (entities[i].generation < ENTITY_GEN_MASK) dead.push(e);
    entities[i].live = false;
}

