#pragma once
#include "engine/registry.hh"

struct Pose;
struct MoveAction;

using Components = TypeList<Pose, MoveAction>;
using GameRegistry = Registry<Components>;

