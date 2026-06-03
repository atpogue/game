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

add_library (${ENGINE})

target_include_directories (
    ${ENGINE}
    PUBLIC ${SDL3_INCLUDE_DIRS}
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    find_library(STDCXX_EXP NAMES stdc++exp)
    if(STDCXX_EXP)
        target_link_libraries(${ENGINE} PRIVATE ${STDCXX_EXP})
        target_compile_definitions(${ENGINE} PRIVATE HAVE_STACKTRACE)
    endif()
endif()

target_link_libraries (
    ${ENGINE}
    PUBLIC SDL3::SDL3
    PUBLIC glm::glm
)

target_compile_options (${ENGINE} PRIVATE -Wall -Wextra -Wpedantic)

add_subdirectory (src)

if (BUILD_TESTS)
    add_subdirectory (tests)
endif()

