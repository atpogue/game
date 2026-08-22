add_library (gfx)

target_include_directories (
  gfx
  PRIVATE ${SDL3_INCLUDE_DIRS}
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries (
  gfx
  PRIVATE SDL3::SDL3
  PUBLIC glm::glm core sys
)

add_subdirectory (src)
