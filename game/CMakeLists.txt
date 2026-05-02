add_executable (${GAME})

target_include_directories (${GAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries (${GAME} PRIVATE ${ENGINE})

target_compile_options (${GAME} PRIVATE -Wall -Wextra -Wpedantic)

add_subdirectory (src)
add_subdirectory (demos)
