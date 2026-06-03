list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)

add_executable(
    test-engine
    registry.cc
)

target_include_directories(test-engine PRIVATE ${catch2_INCLUDE_DIRS}/src)

target_link_libraries(
    test-engine
    PRIVATE ${ENGINE}
    PRIVATE Catch2::Catch2WithMain
)

include(CTest)
include(Catch)
catch_discover_tests(test-engine)

