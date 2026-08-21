add_library (core)

target_include_directories (core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    find_library (STDCXX_EXP NAMES stdc++exp)
    if (STDCXX_EXP)
        target_link_libraries (core PRIVATE ${STDCXX_EXP})
        target_compile_definitions (core PRIVATE HAVE_STACKTRACE)
    endif()
endif()

add_subdirectory (src)

if (BUILD_TESTS)
  add_subdirectory (tests)
endif()

