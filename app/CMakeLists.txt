add_library (app)

target_include_directories (
  app
  PUBLIC ${SDL3_INCLUDE_DIRS}
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries (
  app
  PUBLIC SDL3::SDL3
  PUBLIC glm::glm
  PUBLIC core
)

add_subdirectory (src)

