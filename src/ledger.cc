#include "ledger.hh"
#include <array>
#include <cassert>
#include <initializer_list>
#include <queue>

// problem: signature may not be valid if ledger::sign is used to set components it doesn't have or unset components it does
// problem: destroying an entity doesn't destroy its components
// use a callback?

constexpr Entity ENTITY_ID_BITS  = 20; // 1,048,575 IDs
constexpr Entity ENTITY_GEN_BITS = 12; // 4,095 generations
constexpr Entity ENTITY_ID_MAX  = (1 << ENTITY_ID_BITS) - 1;
constexpr Entity ENTITY_GEN_MAX = (1 << ENTITY_GEN_BITS) - 1;

constexpr uint32_t entity_id(Entity e) { return e & ENTITY_ID_MAX; }
constexpr uint16_t entity_gen(Entity e) { return (e >> ENTITY_ID_BITS) & ENTITY_GEN_MAX; }

struct EntityData {
    bool live = false;
    uint16_t generation;
    Signature signature;
};

namespace {
    std::vector<EntityData> entities(1, EntityData{false,0});
    std::queue<Entity> recycle;
    const char *err_msg = nullptr;
}

Signature signature(std::initializer_list<Component> components) {
    // initializer lists can be constexpr in C++26
    Signature sign;
    for (Component c : components) sign.set(component_index(c), true);
    return sign;
}

const char *ledger::error() {
    auto msg = err_msg;
    err_msg = nullptr;
    return msg;
}

bool ledger::status(Entity e) {
    auto i = entity_id(e);
    if (i >= entities.size()) return false;
    return entities[i].live && entities[i].generation == entity_gen(e);
}

Signature ledger::signature(Entity e) {
    return ledger::status(e) ? entities[entity_id(e)].signature : Signature();
}

bool ledger::has(Entity e, Component c) {
    return ledger::status(e) ? entities[entity_id(e)].signature.test(component_index(c)) : false;
}

bool ledger::has(Entity e, Signature sign) {
    return ledger::status(e) ? (ledger::signature(e) & sign) == sign : false; 
}

bool ledger::has(Entity e, std::initializer_list<Component> sign) {
    return has(e, signature(sign));
}

bool ledger::sign(Entity e, Component c, bool value) {
    if (!ledger::status(e)) return false;
    entities[entity_id(e)].signature.set(component_index(c), value);
    return true;
}

Entity ledger::create() {
    Entity e = NULL_ENTITY;
    if (!recycle.empty()) {
        auto id = entity_id(recycle.front());
        recycle.pop();
        uint16_t gen = entities[id].generation + 1;
        if (gen > ENTITY_GEN_MAX) {
            assert(!"entity with max generation should not be queued for recycle");
            return ledger::create();
        }
        entities[id] = EntityData{true, gen};
        e = (gen << ENTITY_ID_BITS) | id;
    } else if (entities.size() < ENTITY_ID_MAX) {
        auto id = entities.size();
        entities.emplace_back(true, 0);
        e = id;
    } else {
        err_msg = "ledger::create: entity limit reached";
    }
    return e;
}

void ledger::destroy(Entity e) {
    if (!ledger::status(e)) return;
    auto i = entity_id(e);
    if (entities[i].generation < ENTITY_GEN_MAX) recycle.push(e);
    entities[i].live = false;
    entities[i].signature.reset();
}

void ledger::for_each(Signature sign, std::function<void(Entity)> call) {
    // consider using template or function pointer to avoid heap allocation done by std::function
    for (auto id = 1u; id < entities.size(); id++) {
        if ((entities[id].signature & sign) == sign) {
            call((entities[id].generation << ENTITY_ID_BITS) | id);
        }
    }
}

void ledger::for_each(std::initializer_list<Component> sign, std::function<void(Entity)> call) {
    for_each(signature(sign), call);
}

ledger::View ledger::view(Signature sign) {
    std::vector<Entity> matches;
    ledger::for_each(sign,
        [&matches](Entity e) {
            matches.push_back(e);
        }
    );
    return matches;
}

ledger::View ledger::view(std::initializer_list<Component> sign) {
    return view(signature(sign));
}

