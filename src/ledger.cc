#include "ledger.hh"
#include <vector>
#include <queue>

struct EntityData {
    bool status;
    Signature sign;
    Index index;
};

namespace {
    std::vector<EntityData> entities(1, EntityData{false});
    std::queue<Entity> dead;
}

// the issue of someone continueing to use a entity after it is released

// constant functions can't be distinguished, unless I implement a lock functionality... how weird would that be?

// if I bounds check, do I need at?

bool ledger::status(Entity e) {
    return e < entities.size() ? entities[e].status : false;
}

bool ledger::has(Entity e, Component c) {
    return ledger::status(e) ? entities[e].sign.test(c) : false;
}

// is this okay? it would mean that out-of-bounds entity would fail silently, not indicate failure, but it also means that
// a value is returned, not pointer to value, the validity of e can be checked via `ledger::status(e)` or `ledger::is_alive(e)`.
// It's like a "you give trash, you get trash" agreement.
Signature ledger::signature(Entity e) {
    return ledger::status(e) ? entities[e].sign : entities[NULL_ENTITY].sign;
}

Index ledger::index(Entity e) {
    return ledger::status(e) ? entities[e].index : entities[NULL_ENTITY].index;
}

size_t ledger::index(Entity e, Component c) {
    return ledger::status(e) ? entities[e].index[c] : entities[NULL_ENTITY].index[c];
}

bool ledger::assign(Entity e, Component c, size_t i) {
    if (!ledger::status(e)) return false;
    entities[e].sign.set(c, true);
    return true;
}

Entity ledger::create() {
    Entity e = NULL_ENTITY;
    if (!dead.empty()) {
        e = dead.front();
        dead.pop();
        entities[e] = EntityData{true};
    } else {
        e = entities.size();
        entities.emplace_back(true);
    }
    return e;
}

void ledger::destroy(Entity e) {
    if (!ledger::status(e)) return;
    entities[e].status = false;
    dead.push(e);
}

