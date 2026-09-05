find_package(Lua 5.4 REQUIRED)

add_library (sdk)

target_include_directories (
  sdk
  PRIVATE ${LUA_INCLUDE_DIR}
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries (
  sdk
  PRIVATE ${LUA_LIBRARIES}
  PUBLIC core
)

add_subdirectory (src)
