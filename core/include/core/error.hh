#pragma once
#include <expected>
#include <string>

struct Error
{
  std::string msg;
};

template <typename Type>
using Result = std::expected<Type, Error>;
