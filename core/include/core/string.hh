#pragma once
#include "core/hash.hh"
#include <string>
#include <unordered_map>

// A transparent hash allows efficient lookup with string-views without
// constructing a heap-allocated string object during lookup.
template <typename Type>
using StringMap
  = std::unordered_map<std::string, Type, TransparentHash<std::string_view>, std::equal_to<>>;

