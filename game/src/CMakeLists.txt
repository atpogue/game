add_subdirectory (action)
add_subdirectory (component)
add_subdirectory (world)
target_sources (${GAME} PRIVATE main.cc command.cc director.cc sprite.cc)
