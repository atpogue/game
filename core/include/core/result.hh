#pragma once
#include <expected>
#include <string>

template <typename Type>
using Result = std::expected<Type, std::string>;
using Error  = std::unexpected<std::string>;
using Status = std::expected<void, std::string>;

template <typename Type>
Error err(std::string_view msg)
{
  return Error(std::in_place, msg);
}

template <typename Type>
Error err(Result<Type>& result)
{
  return std::move(result).error();
}
