#pragma once
#include "engine/core/grid2.hh"

void random_fill(Grid2<u8> &out, u64 seed, f32 chance);
void generate_caves(Grid2<u8> &out, i64 iterations=4, i64 threshold=5, i64 range=1);

