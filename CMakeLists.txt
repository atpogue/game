cmake_minimum_required (VERSION 3.24)

set (CMAKE_CXX_STANDARD 23)
set (CMAKE_COLOR_DIAGNOSTICS ON)
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")
set (CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}" CACHE INTERNAL "") # prevent installing to system directories

project (game VERSION 0.0.1 LANGUAGES CXX)

set (ENGINE engine)
set (GAME ${PROJECT_NAME})

add_subdirectory (game)
add_subdirectory (engine)

