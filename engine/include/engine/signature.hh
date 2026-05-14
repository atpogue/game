#pragma once
#include "engine/core/type-flag.hh"

struct Actor;
struct Pose;

using Signature = TypeFlag<Actor, Pose>;
using Components = Signature::Types;

