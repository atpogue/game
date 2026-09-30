#pragma once
#include "app/lua/types.hh"
#include "core/panic.hh"
#include <string_view>

namespace Lua {

  // Identifies a field of a table by name or by integer index.
  // Note: Key does not own or manage a copy of the string value. It holds a string view.
  struct Key
  {
    enum class Kind : u8 { Index, Name };

    constexpr Key(char const* name) noexcept : kind_{ Kind::Name }, name_{ name } {}

    constexpr Key(std::string_view name) noexcept : kind_{ Kind::Name }, name_{ name } {}

    constexpr Key(Integer index) noexcept : kind_{ Kind::Index }, index_{ index } {}

    constexpr Key(int index) noexcept : Key(Integer{ index }) {}

    [[nodiscard]] constexpr std::string_view as_name() const noexcept
    {
      DEBUG_ASSERT(kind_ == Kind::Name);
      return name_;
    }

    [[nodiscard]] constexpr Integer as_index() const noexcept
    {
      DEBUG_ASSERT(kind_ == Kind::Index);
      return index_;
    }

    [[nodiscard]] constexpr bool is_index() const noexcept { return kind_ == Kind::Index; }

    [[nodiscard]] constexpr bool is_name() const noexcept { return kind_ == Kind::Name; }

    [[nodiscard]] constexpr Kind kind() const noexcept { return kind_; }

  private:

    Kind             kind_  = Kind::Index;
    Integer          index_ = 0;
    std::string_view name_  = {};
  };

} // namespace Lua
