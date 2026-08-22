find_package(Lua 5.4 REQUIRED)

add_library (game)

target_include_directories (
  game
  PRIVATE ${LUA_INCLUDE_DIR}
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries (
  game
  PRIVATE ${LUA_LIBRARIES}
  PUBLIC core gfx
)

add_subdirectory (src)
