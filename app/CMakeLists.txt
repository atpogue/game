add_library (app)

target_include_directories (
  app
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries (
  app
  PRIVATE sys gfx
  PUBLIC core game
)

add_subdirectory (src)
