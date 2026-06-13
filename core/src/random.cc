#include "core/random.hh"
#include "core/types.hh"
#include <random>

namespace {
  constexpr u64 rotl(u64 x, int k) { return (x << k) | (x >> (64 - k)); }
} // namespace

u64 random_seed()
{
  std::random_device rd;
  return (u64{rd()} << 32) | rd();
}

Xoshiro256ss::Xoshiro256ss(u64 seed)
{
  SplitMix64 rng(seed);
  for (int i = 0; i < 4; ++i) s[i] = rng();
}

u64 Xoshiro256ss::operator()()
{
  u64 const result  = rotl(s[1] * 5, 7) * 9;
  u64 const t       = s[1] << 17;
  s[2]             ^= s[0];
  s[3]             ^= s[1];
  s[1]             ^= s[2];
  s[0]             ^= s[3];
  s[2]             ^= t;
  s[3]              = rotl(s[3], 45);
  return result;
}
