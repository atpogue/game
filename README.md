## Building 

### Requirements

- A C++23 compiler: GCC 13+, Clang 17+, or MSVC 19.38+
- [CMake](https://cmake.org/) 3.25+ 
- [Ninja](https://ninja-build.org/)

### Dependencies

These libraries will be fetched and built from source via CMake's `FetchContent` if not found on the system.

- [SDL3](https://www.libsdl.org/)
- [GLM](https://www.opengl.org/sdk/libs/GLM/)

### Commands

To build the project:
```bash
cmake --preset debug && cmake --build --preset debug
```

To run the game:
```bash
./build/debug/game/game
```

