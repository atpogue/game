cmake_minimum_required (VERSION 3.16)

project (game VERSION 0.0.1 LANGUAGES CXX)

set (CMAKE_CXX_STANDARD 20)
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

include (FetchContent)

find_package (SDL3 QUIET CONFIG)

if (NOT SDL3_FOUND)
    message (STATUS "SDL3 not found, fetching...")
    set (CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}" CACHE INTERNAL "") # prevent installing to system directories
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
else ()
    message (STATUS "SDL3 found: ${SDL3_DIR}")
endif ()

add_executable (${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC ${SDL3_INCLUDE_DIRS})

add_subdirectory (src)

target_link_libraries (
    ${PROJECT_NAME}
    PUBLIC SDL3::SDL3
)
