cmake_minimum_required (VERSION 3.25)

project (game VERSION 0.0.1 LANGUAGES CXX)

set (ENGINE engine)
set (GAME ${PROJECT_NAME})

add_subdirectory (game)
add_subdirectory (engine)

