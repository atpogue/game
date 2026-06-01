#include "engine/core/error.hh"
#include <cstdlib>
#include <SDL3/SDL_log.h>

#ifdef HAVE_STACKTRACE
#include <stacktrace>
#endif

void detail::fatal_error(
    const char *category,
    const char *condition,
    const char *message,
    std::source_location location
) {
#ifdef HAVE_STACKTRACE
    std::stacktrace trace  = std::stacktrace::current();
#endif
    SDL_LogCritical(
        SDL_LOG_CATEGORY_ERROR,
        "[%s] %s\n"
        "  condition : %s\n"
        "  location  : %s:%u in %s\n"
#ifdef HAVE_STACKTRACE
        "  stacktrace:\n%s"
#endif
        ,
        category,
        message,
        condition,
        location.file_name(), location.line(), location.function_name()
#ifdef HAVE_STACKTRACE
        , std::to_string(trace).c_str()
#endif
    );
    std::abort();
}

