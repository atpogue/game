cmake_minimum_required (VERSION 3.25)

project (game VERSION 0.0.1 LANGUAGES CXX)

set (CORE core)
set (ENGINE engine)
set (GAME ${PROJECT_NAME})

option (BUILD_TESTS "Build tests" OFF)
option (ENABLE_CLANG_TIDY "Run clang-tidy on build" OFF)

include (FetchContent)

if (BUILD_TESTS)
    enable_testing ()
    FetchContent_Declare (
        Catch2
        GIT_REPOSITORY      https://github.com/catchorg/Catch2.git
        GIT_TAG             v3.15.0
        GIT_SHALLOW         TRUE
        FIND_PACKAGE_ARGS   3.0.0
    )
    FetchContent_MakeAvailable (Catch2)
endif ()

add_subdirectory (core)
add_subdirectory (engine)
add_subdirectory (game)

if (ENABLE_CLANG_TIDY)
    find_program (CLANG_TIDY_EXE NAMES clang-tidy REQUIRED)
    set_target_properties (${ENGINE} PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_EXE}")
    set_target_properties (${GAME}   PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_EXE}")
endif ()

