add_library (game)

target_include_directories (
  game
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries (
  game
  PUBLIC core gfx sdk
)

add_subdirectory (src)
