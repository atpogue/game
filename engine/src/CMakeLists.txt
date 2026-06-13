add_subdirectory (input)
add_subdirectory (render)
add_subdirectory (ui)
target_sources (${ENGINE} PRIVATE engine.cc event.cc)
