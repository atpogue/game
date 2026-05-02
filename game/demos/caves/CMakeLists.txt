add_executable (caves)

set_target_properties (
    caves PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/demos"
)

target_include_directories (
    caves
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE "${CMAKE_SOURCE_DIR}/game/src"
)

target_link_libraries (caves PRIVATE ${ENGINE})

target_compile_options (caves PRIVATE -Wall -Wextra -Wpedantic)

target_sources (caves PRIVATE main.cc "${CMAKE_SOURCE_DIR}/game/src/caves.cc")
