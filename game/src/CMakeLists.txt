add_subdirectory (action)
add_subdirectory (component)
add_subdirectory (world)
target_sources (game PRIVATE game.cc data.cc command.cc command-buffer.cc user-interface.cc pilot.cc sprite.cc catalog.cc registry.cc entity.cc context.cc scene.cc lua.cc)
