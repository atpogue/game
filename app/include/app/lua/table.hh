#pragma once

// TODO: iterate over key/value pairs without collecting the keys first

#include "app/lua/key.hh"
#include "app/lua/reference.hh"
#include "app/lua/value.hh"
#include <string>
#include <string_view>
#include <vector>

namespace Lua {

  struct Table;

  template <typename Type>
  concept TableReadable = requires (Type& dst, Table const& src) {
    { read(dst, src) } -> std::same_as<bool>;
  };

  // A table held by a Lua state, annotated with the path it was reached from.
  // Must not outlive the state that created it.
  // Access is raw: metamethods are ignored.
  struct Table
  {
    // Assumes: the reference holds a table.
    Table(Reference reference, std::string path) noexcept;

    template <TableReadable Type>
    [[nodiscard]] bool read(Type& dst) const
    {
      return detail::dispatch_read(dst, *this);
    }

    // Returns a nil value on a missing field.
    [[nodiscard]] Value operator[](Key key) const;

    void set(Key key, bool value) const noexcept;
    void set(Key key, Number value) const noexcept;
    void set(Key key, Integer value) const noexcept;
    void set(Key key, std::string_view value) const noexcept;

    void set(Key key, int value) const noexcept { set(key, Integer{ value }); }

    // Prevents string literals from decaying into booleans.
    void set(Key key, char const* value) const noexcept { set(key, std::string_view(value)); }

    void set(Key key, Table const& value) const noexcept { set(key, value.reference_); }

    void set(Key key, Value const& value) const noexcept { set(key, value.reference()); }

    // Assumes: the reference was produced by the same state.
    void set(Key key, Reference const& value) const noexcept;

    void erase(Key key) const noexcept;

    // Returns the integer and string keys of the table in a deterministic order: integers in
    // ascending order followed by names in lexicographic order. Keys of other types are skipped
    // with a diagnostic.
    // Note: names view strings owned by the Lua state and remain valid while the table holds them.
    [[nodiscard]] std::vector<Key> keys() const;

    [[nodiscard]] std::string_view path() const noexcept { return path_; }

    [[nodiscard]] Reference const& reference() const noexcept { return reference_; }

  private:

    Reference   reference_;
    std::string path_;
  };

} // namespace Lua
