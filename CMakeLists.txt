cmake_minimum_required (VERSION 3.25)

project (game VERSION 0.0.1 LANGUAGES CXX)

set (CORE core)
set (ENGINE engine)
set (GAME ${PROJECT_NAME})

option (BUILD_TESTS "Build tests" OFF)
option (ENABLE_LINTER "Run fast static analysis (clang-tidy) during builds" OFF)
option (ENABLE_DEEP_ANALYSIS "Run slow, deep static analysis (-fanalyzer or /analyze)" OFF)

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

add_library(project_warnings INTERFACE)

if (MSVC)
    target_compile_options(project_warnings INTERFACE /W4)
else ()
    target_compile_options(project_warnings INTERFACE -Wall -Wextra -Wpedantic)
endif()

if (ENABLE_DEEP_ANALYSIS)
    if (MSVC)
        target_compile_options (project_warnings INTERFACE /analyze)
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_compile_options (project_warnings INTERFACE -fanalyzer)
    else ()
        message (FATAL_ERROR "ENABLE_DEEP_ANALYSIS is ON, but ${CMAKE_CXX_COMPILER_ID} doesn't have an analyzer.")
    endif ()
endif ()

target_link_libraries(${CORE}   PRIVATE project_warnings)
target_link_libraries(${ENGINE} PRIVATE project_warnings)
target_link_libraries(${GAME}   PRIVATE project_warnings)

if (ENABLE_LINTER)
    find_program(CLANG_TIDY_EXE NAMES clang-tidy REQUIRED)
    set_target_properties (${CORE}   PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_EXE}")
    set_target_properties (${ENGINE} PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_EXE}")
    set_target_properties (${GAME}   PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_EXE}")
endif ()

