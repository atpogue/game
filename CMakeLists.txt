cmake_minimum_required (VERSION 3.25)

project (game VERSION 0.0.1 LANGUAGES CXX)

option (BUILD_TESTS "Build tests" OFF)
option (ENABLE_LINTER "Run fast static analysis (clang-tidy) during builds" OFF)
option (ENABLE_DEEP_ANALYSIS "Run slow, deep static analysis (-fanalyzer or /analyze)" OFF)

include (FetchContent)

FetchContent_Declare (
  SDL3
  GIT_REPOSITORY      https://github.com/libsdl-org/SDL.git
  GIT_TAG             release-3.4.10
  GIT_SHALLOW         TRUE
  FIND_PACKAGE_ARGS   3.4.0
)
set (DSDL_SHARED ON)
set (DSDL_STATIC OFF)
set (DSDL_TEST_LIBRARY OFF)
FetchContent_MakeAvailable (SDL3)

FetchContent_Declare (
  glm
  GIT_REPOSITORY      https://github.com/g-truc/glm.git
  GIT_TAG             1.0.3
  GIT_SHALLOW         TRUE
  FIND_PACKAGE_ARGS   1.0.0
)
FetchContent_MakeAvailable (glm)

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
add_subdirectory (app)
add_subdirectory (gfx)
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

target_link_libraries (core PRIVATE project_warnings)
target_link_libraries (app  PRIVATE project_warnings)
target_link_libraries (gfx  PRIVATE project_warnings)
target_link_libraries (game PRIVATE project_warnings)

if (ENABLE_LINTER)
  find_program(CLANG_TIDY_EXE NAMES clang-tidy REQUIRED)
  set_target_properties (core PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_EXE}")
  set_target_properties (app  PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_EXE}")
  set_target_properties (gfx  PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_EXE}")
  set_target_properties (game PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_EXE}")
endif ()

