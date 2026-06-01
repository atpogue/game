#pragma once
#include <source_location>
  
namespace detail {

    [[noreturn]] void fatal_error(
        const char *category,
        const char *condition,
        const char *message,
        std::source_location location = std::source_location::current()
    );

} // namespace detail

#define PASS_OR_ABORT(category, cond, ...)                          \
    do {                                                            \
        if (!(cond)) [[unlikely]]                                   \
            detail::fatal_error(category, #cond, "" __VA_ARGS__);   \
    } while(0)

// Always-on: aborts in both debug and release.
#define PANIC(cond, ...)        PASS_OR_ABORT("PANIC", cond, ##__VA_ARGS__)
#define PRECONDITION(cond, ...) PASS_OR_ABORT("PRECONDITION", cond, ##__VA_ARGS__)
#define INVARIANT(cond, ...)    PASS_OR_ABORT("INVARIANT", cond, ##__VA_ARGS__)

// Debug-only: aborts in debug, zero-cost in release
#ifdef NDEBUG
    #define ASSERT(cond, ...) ((void)0)
#else
    #define ASSERT(cond, ...) PASS_OR_ABORT("ASSERT", cond, ##__VA_ARGS__)
#endif

