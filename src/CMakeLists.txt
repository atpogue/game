#file (GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS *.c)
#file (GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS *.cc)

set (SOURCE_FILES main.cc menu.cc keyboard.cc mouse.cc cue.cc director.cc)

message (NOTICE "include directory: ${CMAKE_CURRENT_SOURCE_DIR}")
target_include_directories (${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_sources (${PROJECT_NAME} PRIVATE ${SOURCE_FILES})
