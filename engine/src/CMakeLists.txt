add_subdirectory (input)
add_subdirectory (render)
target_sources (${ENGINE} PRIVATE main.cc event.cc)
