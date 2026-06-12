#pragma once
#include <source_location>
#include <string>
#include <expected>

namespace detail {

    [[noreturn]] void panic(
        const char *category,
        const char *condition,
        const char *message,
        std::source_location location = std::source_location::current()
    );

} // namespace detail

#define PANIC(category, cond, ...)                          \
    do {                                                    \
        if (!(cond)) [[unlikely]]                           \
            detail::panic(category, #cond, "" __VA_ARGS__); \
    } while(0)

// Always-on: aborts in both debug and release.
#define ASSERT(cond, ...)       PANIC("ASSERTION", cond __VA_OPT__(,) __VA_ARGS__)
#define PRECONDITION(cond, ...) PANIC("PRECONDITION", cond __VA_OPT__(,) __VA_ARGS__)
#define INVARIANT(cond, ...)    PANIC("INVARIANT", cond __VA_OPT__(,) __VA_ARGS__)

// Debug-only: aborts in debug, zero-cost in release
#ifdef NDEBUG
    #define DEBUG_ASSERT(cond, ...) ((void)0)
#else
    #define DEBUG_ASSERT(cond, ...) PANIC("ASSERTION", cond __VA_OPT__(,) __VA_ARGS__)
#endif

struct Error { std::string msg; };

template <typename Type>
using Result = std::expected<Type, Error>;

