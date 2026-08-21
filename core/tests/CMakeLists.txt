list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)

add_executable(test-core)

target_sources (test-core PRIVATE registry.cc)

target_include_directories(test-core PRIVATE ${catch2_INCLUDE_DIRS}/src)

target_link_libraries(
    test-core
    PRIVATE core
    PRIVATE Catch2::Catch2WithMain
)

include(CTest)
include(Catch)
catch_discover_tests(test-core)

