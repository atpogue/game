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

add_library (${ENGINE})

target_include_directories (
    ${ENGINE}
    PUBLIC ${SDL3_INCLUDE_DIRS}
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries (
    ${ENGINE}
    PUBLIC ${CORE}
    PUBLIC SDL3::SDL3
)

add_subdirectory (src)

