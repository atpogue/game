#include "core/random.hh"
#include "core/types.hh"

namespace {
    constexpr u64 rotl(u64 x, int k) {
        return (x << k) | (x >> (64 - k));
    }
}

Xoshiro256ss::Xoshiro256ss(u64 seed) {
    SplitMix64 rng(seed);
    for (int i = 0; i < 4; ++i) s[i] = rng();
}

u64 Xoshiro256ss::operator()() {
    const u64 result = rotl(s[1] * 5, 7) * 9;
    const u64 t = s[1] << 17;
    s[2] ^= s[0];
    s[3] ^= s[1];
    s[1] ^= s[2];
    s[0] ^= s[3];
    s[2] ^= t;
    s[3] = rotl(s[3], 45);
    return result;
}

