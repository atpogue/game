#include "engine/core/types.hh"
#include "engine/core/hash.hh"

u64 random_seed();

// Fixed-increment version of Java 8's SplittableRandom generator.
// Written by Sebastiano Vigna (https://prng.di.unimi.it/splitmix64.c).
struct SplitMix64 {

    using result_type = u64;

    static constexpr u64 min() { return 0; }
    static constexpr u64 max() { return UINT64_MAX; }

    explicit SplitMix64(u64 seed) : x{seed} {}
    inline u64 operator()() {
        x += 0x9e3779b97f4a7c15ULL;
        return split_mix(x);
    }

private:

    u64 x;

};

// All-purpose psuedo-random number generator.
// Written by David Blackman and Sebastiano Vigna (https://prng.di.unimi.it/).
struct Xoshiro256ss {

    using result_type = u64;

    static constexpr u64 min() { return 0; }
    static constexpr u64 max() { return UINT64_MAX; }

    explicit Xoshiro256ss(u64 seed);
    u64  operator()();

private:

    u64 s[4];

};

