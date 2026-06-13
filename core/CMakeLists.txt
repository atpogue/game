find_package(Lua 5.4 REQUIRED)

include (FetchContent)

FetchContent_Declare (
    glm
    GIT_REPOSITORY      https://github.com/g-truc/glm.git
    GIT_TAG             1.0.3
    GIT_SHALLOW         TRUE
    FIND_PACKAGE_ARGS   1.0.0
)
FetchContent_MakeAvailable (glm)

add_library (${CORE})

target_include_directories (
    ${CORE}
    PUBLIC ${LUA_INCLUDE_DIR}
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    find_library(STDCXX_EXP NAMES stdc++exp)
    if(STDCXX_EXP)
        target_link_libraries(${CORE} PRIVATE ${STDCXX_EXP})
        target_compile_definitions(${CORE} PRIVATE HAVE_STACKTRACE)
    endif()
endif()

target_link_libraries (
    ${CORE}
    PUBLIC glm::glm
    PUBLIC ${LUA_LIBRARIES}
)

target_compile_options (${CORE} PRIVATE -Wall -Wextra -Wpedantic)

add_subdirectory (src)

if (BUILD_TESTS)
    add_subdirectory (tests EXCLUDE_FROM_ALL)
endif()

