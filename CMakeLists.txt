cmake_minimum_required (VERSION 3.24)

set (CMAKE_CXX_STANDARD 23)
set (CMAKE_COLOR_DIAGNOSTICS ON)
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")
set (CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}" CACHE INTERNAL "") # prevent installing to system directories

project (game VERSION 0.0.1 LANGUAGES CXX)

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

FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.3
    GIT_SHALLOW TRUE
    FIND_PACKAGE_ARGS
)
FetchContent_MakeAvailable(glm)

add_executable (${PROJECT_NAME})

target_include_directories (${PROJECT_NAME} PUBLIC ${SDL3_INCLUDE_DIRS})
target_include_directories (${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src)

add_subdirectory (src)

target_link_libraries (
    ${PROJECT_NAME}
    PRIVATE SDL3::SDL3
    PRIVATE glm::glm
)
