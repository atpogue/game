#pragma once
#include "app/lua/reference.hh"
#include "app/lua/types.hh"
#include <concepts>
#include <limits>
#include <optional>
#include <string>
#include <string_view>
#include <utility>

namespace Lua {
  struct Table;
  struct Value;

  // Readers translate a Lua value into a C++ object. On failure, they print a diagnostic that
  // includes the value's path and return false. Overloads for other types can be declared in
  // namespace Lua and will be found by `Value::read`.

  bool read(bool& dst, Value const& src);
  bool read(Integer& dst, Value const& src);
  bool read(Number& dst, Value const& src);
  bool read(std::string& dst, Value const& src);

  template <std::integral Type>
  bool read(Type& dst, Value const& src);

  template <std::floating_point Type>
  bool read(Type& dst, Value const& src);

  template <typename Type>
  concept ValueReadable = requires (Type& dst, Value const& src) {
    { read(dst, src) } -> std::same_as<bool>;
  };

  namespace detail {
    // Calls `read` from outside the scope of the member functions of the same name, which would
    // otherwise hide the free functions.
    template <typename Type, typename Source>
    bool dispatch_read(Type& dst, Source const& src)
    {
      return read(dst, src);
    }
  }

  // A value held by a Lua state, annotated with the path it was reached from.
  // Must not outlive the state that created it.
  struct Value
  {
    Value(Reference reference, std::string path) noexcept;

    template <ValueReadable Type>
    [[nodiscard]] bool read(Type& dst) const
    {
      return detail::dispatch_read(dst, *this);
    }

    // Intended to be called once the type is known (i.e. in a switch statement over type).
    // In debug: asserts on type mismatch.
    // In release: same behavior as the equivalent Lua C API.
    [[nodiscard]] bool             to_boolean() const noexcept;
    [[nodiscard]] Number           to_number() const noexcept;
    [[nodiscard]] Integer          to_integer() const noexcept;
    [[nodiscard]] std::string_view to_string() const noexcept; // valid while this value is alive
    [[nodiscard]] Table            to_table() const;

    // Print a diagnostic and return nothing if the value is not of the expected type.
    [[nodiscard]] std::optional<bool>    expect_boolean() const;
    [[nodiscard]] std::optional<Integer> expect_integer() const;
    [[nodiscard]] std::optional<Integer> expect_integer_range(Integer min, Integer max) const;
    [[nodiscard]] std::optional<Number>  expect_number() const;
    [[nodiscard]] std::optional<Number>  expect_number_range(Number min, Number max) const;
    [[nodiscard]] std::optional<std::string_view> expect_string() const;
    [[nodiscard]] std::optional<Table>            expect_table() const;

    [[nodiscard]] Type type() const noexcept { return reference_.type(); }

    [[nodiscard]] bool is_nil() const noexcept { return type() == Type::Nil; }

    [[nodiscard]] bool is_boolean() const noexcept { return type() == Type::Boolean; }

    [[nodiscard]] bool is_integer() const noexcept { return type() == Type::Integer; }

    // Note: integers are distinguished from other numbers; `is_number` is false for integers.
    [[nodiscard]] bool is_number() const noexcept { return type() == Type::Number; }

    [[nodiscard]] bool is_string() const noexcept { return type() == Type::String; }

    [[nodiscard]] bool is_table() const noexcept { return type() == Type::Table; }

    // Describes the type and, for scalar types, the value (i.e. `integer 42`).
    [[nodiscard]] std::string describe() const;

    [[nodiscard]] std::string_view path() const noexcept { return path_; }

    [[nodiscard]] Reference const& reference() const noexcept { return reference_; }

  private:

    // Prints a type mismatch diagnostic.
    void report(std::string_view expected) const;

    Reference   reference_;
    std::string path_;
  };

  template <std::integral Type>
  bool read(Type& dst, Value const& src)
  {
    // Clamp the destination type's range to the range representable by a Lua integer.
    constexpr Integer min
      = std::cmp_less(std::numeric_limits<Type>::lowest(), std::numeric_limits<Integer>::lowest())
        ? std::numeric_limits<Integer>::lowest()
        : static_cast<Integer>(std::numeric_limits<Type>::lowest());
    constexpr Integer max
      = std::cmp_greater(std::numeric_limits<Type>::max(), std::numeric_limits<Integer>::max())
        ? std::numeric_limits<Integer>::max()
        : static_cast<Integer>(std::numeric_limits<Type>::max());
    std::optional<Integer> integer = src.expect_integer_range(min, max);
    if (!integer) return false;
    dst = static_cast<Type>(*integer);
    return true;
  }

  template <std::floating_point Type>
  bool read(Type& dst, Value const& src)
  {
    std::optional<Number> number = src.expect_number_range(
      std::numeric_limits<Type>::lowest(), std::numeric_limits<Type>::max());
    if (!number) return false;
    dst = static_cast<Type>(*number);
    return true;
  }

} // namespace Lua
