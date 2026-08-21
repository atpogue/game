find_package(Lua 5.4 REQUIRED)

add_executable (game)

target_include_directories (
  game
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
  PRIVATE ${LUA_INCLUDE_DIR}
)

target_link_libraries (
  game
  PRIVATE ${LUA_LIBRARIES}
  PRIVATE app
  PRIVATE gfx
  PRIVATE core
)

add_subdirectory (src)
# add_subdirectory (demos)

