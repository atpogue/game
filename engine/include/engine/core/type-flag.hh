#pragma once
#include "engine/core/type-list.hh"
#include <bitset>

template <typename... Ts>
struct TypeFlag : private std::bitset<sizeof...(Ts)> {
private:

    using Base = std::bitset<sizeof...(Ts)>;

public:

    using Types = TypeList<Ts...>;

    using Base::set;
    using Base::any;
    using Base::none;
    using Base::count;
    using Base::size;
    using Base::reset;

    template <typename U, typename... Us>
    constexpr void set(bool value = true) {
        bits_.set(Types::template index_of<U>(), value);
        (bits_.set(Types::template index_of<Us>(), value), ...);
    }

    constexpr bool has(u32 i) const { return bits_.test(i); }

    constexpr bool has(const TypeFlag &other) const {
        return (bits_ & other.bits_) == other.bits_;
    }

    template <typename U, typename... Us>
    constexpr bool has() {
        return bits_.test(Types::template index_of<U>())
            && (bits_.test(Types::template index_of<Us>()) && ...);
    }

private:

    std::bitset<Types::size> bits_;

};

