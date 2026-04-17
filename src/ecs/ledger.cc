#include "ecs/ledger.hh"
#include "core/slot-map.hh"
#include <initializer_list>

// problem: signature may not be valid if ledger::sign is used to set components it doesn't have or unset components it does
// problem: destroying an entity doesn't destroy its components
// use a callback?

namespace {
    SlotMap<Signature> data;
    const char *err_msg = nullptr;
}

Signature signature(std::initializer_list<Flag> flags) {
    // initializer lists can be constexpr in C++26
    Signature sign;
    for (Flag f : flags) sign.set(flag_index(f), true);
    return sign;
}

const char *ledger::error() {
    auto msg = err_msg;
    err_msg = nullptr;
    return msg;
}

bool ledger::status(Entity e) { return data.status(e); }

Signature ledger::signature(Entity e) {
    auto ptr = data.get(e);
    return ptr ? *ptr : Signature();
}

bool ledger::has(Entity e, Flag f) {
    auto ptr = data.get(e);
    return ptr ? ptr->test(flag_index(f)) : false;
}

bool ledger::has(Entity e, Signature sign) {
    auto ptr = data.get(e);
    return ptr ? (*ptr & sign) == sign : false; 
}

bool ledger::has(Entity e, std::initializer_list<Flag> sign) {
    return has(e, signature(sign));
}

bool ledger::sign(Entity e, Flag f, bool value) {
    auto ptr = data.get(e);
    if (!ptr) return false;
    ptr->set(flag_index(f), value);
    return true;
}

u32 ledger::generation(u32 index) { return data.generation(index); }

Entity ledger::create() { return data.emplace(); }

void ledger::destroy(Entity e) { data.erase(e); }

void ledger::for_each(Signature match, std::function<void(Entity)> call) {
    // consider using template or function pointer to avoid heap allocation done by std::function
    for (auto [e, sign] : data) {
        if ((sign & match) == match) call(e);
    }
}

void ledger::for_each(std::initializer_list<Flag> sign, std::function<void(Entity)> call) {
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

ledger::View ledger::view(std::initializer_list<Flag> sign) {
    return view(signature(sign));
}

