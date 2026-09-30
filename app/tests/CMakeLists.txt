list (APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)

add_executable (test-app)

target_sources (test-app PRIVATE lua.cc)

target_link_libraries (
  test-app
  PRIVATE Catch2::Catch2WithMain
  PRIVATE app
)

include (CTest)
include (Catch)
catch_discover_tests (test-app)
