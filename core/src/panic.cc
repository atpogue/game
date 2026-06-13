#include "core/panic.hh"
#include <cstdlib>
#include <print>

#ifdef HAVE_STACKTRACE
  #include <stacktrace>
#endif

void detail::panic(
  char const* category, char const* condition, char const* message, std::source_location location
)
{
#ifdef HAVE_STACKTRACE
  std::stacktrace trace = std::stacktrace::current();
#endif
  std::print(
    stderr,
    "[{}] {}\n" "  condition : {}\n" "  location  : {}:{} in {}\n"
#ifdef HAVE_STACKTRACE
    "  stacktrace:\n{}"
#endif
    ,
    category, message, condition, location.file_name(), location.line(), location.function_name()
#ifdef HAVE_STACKTRACE
                                                                           ,
    std::to_string(trace).c_str()
#endif
  );
  std::abort();
}
