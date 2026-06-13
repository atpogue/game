#pragma once
#include "core/types.hh"
#include <glm/common.hpp>

template <typename Type>
using Vec2  = glm::vec<2, Type>;
using Vec2f = Vec2<f32>;
using Vec2i = Vec2<i32>;
using Vec2u = Vec2<u32>;

template <typename Type>
using Vec3  = glm::vec<3, Type>;
using Vec3f = Vec3<f32>;
using Vec3i = Vec3<i32>;
using Vec3u = Vec3<u32>;

template <typename Type>
using Mat4 = glm::mat<4, 4, Type>;
