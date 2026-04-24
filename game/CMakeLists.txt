add_executable (${GAME})

target_include_directories (${GAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries (${GAME} PRIVATE ${ENGINE})

add_subdirectory (src)
