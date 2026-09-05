list (APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)

add_executable (test-sdk)

target_sources (test-sdk PRIVATE lua.cc)

target_link_libraries (
  test-sdk
  PRIVATE sdk
  PRIVATE Catch2::Catch2WithMain
)

include (CTest)
include (Catch)
catch_discover_tests (test-sdk)
