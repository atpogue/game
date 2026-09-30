add_library(game)

target_include_directories(
  game
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(game PUBLIC core glm::glm)

add_subdirectory(src)
