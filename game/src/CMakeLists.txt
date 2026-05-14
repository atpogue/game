add_subdirectory (action)
add_subdirectory (procgen)
target_sources (${GAME} PRIVATE main.cc command.cc director.cc)
