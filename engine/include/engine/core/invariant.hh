#pragma once
#include <SDL3/SDL_log.h>
#include <cassert>
  
#ifdef NDEBUG
    #define INVARIANT(cond, msg) do {                                       \
        if (!(cond)) {                                                      \
            SDL_LogCritical(SDL_LOG_CATEGORY_ERROR,                         \
                "Failed assertion: %s (%s:%d)", msg, __FILE__, __LINE__);   \
            std::abort();                                                   \
        }                                                                   \
    } while(0)
#else
  #define INVARIANT(cond, msg) assert(cond && msg)
#endif

