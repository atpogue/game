## Building 

### Toolchain

- A C++23 compiler: GCC 14+, Clang 17+, or MSVC 19.38+
- [CMake](https://cmake.org/) 3.25+ 
- [Lua](https://www.lua.org/) 5.4+

### Dependencies

If not found on the system, dependencies will be fetched and built from source via CMake's `FetchContent`.

- [SDL3](https://github.com/libsdl-org/SDL) 3.4+
- [GLM](https://github.com/g-truc/glm) 1.0+
- [Catch2](https://github.com/catchorg/Catch2) 3.0+ (Optional, needed to build unit tests)

### Commands

To build the project:
```bash
cmake --preset debug
cmake --build --preset debug
```

To run the game:
```bash
./bin/debug/game
```

