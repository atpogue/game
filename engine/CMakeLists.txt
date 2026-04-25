include (FetchContent)

FetchContent_Declare (
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-3.2.4
    GIT_SHALLOW TRUE
    FIND_PACKAGE_ARGS
)
set (DSDL_SHARED ON)
set (DSDL_STATIC OFF)
set (DSDL_TEST_LIBRARY OFF)
FetchContent_MakeAvailable (SDL3)

FetchContent_Declare (
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.3
    GIT_SHALLOW TRUE
    FIND_PACKAGE_ARGS
)
FetchContent_MakeAvailable (glm)

add_library (${ENGINE})

target_include_directories (${ENGINE} PUBLIC ${SDL3_INCLUDE_DIRS})

target_include_directories (
    ${ENGINE}
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries (
    ${ENGINE}
    PUBLIC SDL3::SDL3
    PUBLIC glm::glm
)

target_compile_options (${ENGINE} PRIVATE -Wall -Wextra -Wpedantic)

add_subdirectory (src)
