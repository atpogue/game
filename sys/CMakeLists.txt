add_library (sys)

target_include_directories (
  sys
  PUBLIC ${SDL3_INCLUDE_DIRS}
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries (
  sys
  PUBLIC SDL3::SDL3
  PUBLIC glm::glm
  PUBLIC core
)

add_subdirectory (src)

