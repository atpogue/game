cmake_minimum_required (VERSION 3.25)

project (game VERSION 0.0.1 LANGUAGES CXX)

set (ENGINE engine)
set (GAME ${PROJECT_NAME})

option (BUILD_TESTS "Build tests" OFF)

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

add_subdirectory (game)
add_subdirectory (engine)

