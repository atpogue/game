# Project Instructions

Record speculative or planned features in `DEFERRED.md` or name them in a `// TODO: ...` comment near related code.

## Project Structure
- Reusable utilities and containers belong in `core/`
- Graphics, input-handling, and platform specific logic belong in `engine/`
- Genre specific utilities and containers belong in `game/`
- Game specific definitions (items, terrain, ...) belong in `content/`
- Respect dependency isolation: `core` requires GLM and Lua, `engine` requires SDL3. Do not put SDL related code in `core`.
- Lower layers must not depend on higher layers:
    - `content` (Lua scripts) depends on `game`
    - `game` depends on `engine`
    - `engine` depends on `core`
    - `core` has no internal dependencies
- Each layer has its own `CMakeLists.txt` and its own compilation target.
- Ask before modifying top-level cmake files: `CMakeLists.txt`, `<layer>/CMakeLists.txt`
- Public Headers: `<layer>/include/<layer>/<subsystem>/`
- Private Headers and Sources: `<layer>/src/<subsystem>/`
- Tests: `<layer>/tests/`
- Add new test files to the relevant `<layer>/tests/CMakeLists.txt`.
- Add new source files to the relevant `<layer>/src/CMakeLists.txt`.

## Toolchain
- Prefer GitHub CLI over git.
- Prefer clang and ninja over gcc. Use the cmake preset `clang-debug`.
    - Note: `.clangd` points at `build/debug`, the `clang-debug` preset compiles to `build/debug` (it inherits the paths set by the `debug` preset)

## Essential Commands
- Configure Project: `cmake --preset clang-debug -DBUILD_TESTS=ON`
- Build Project: `cmake --build --preset clang-debug`
- Run Tests: `ctest --preset clang-debug`
- Format Project: `find . -name '*.cc' -o -name '*.hh' | xargs clang-format -i;`

## Software Design
- Use modern C++23 practices.
- Loosely follow **Data-Oriented Design** principles.
- Use **Design by Contract** programming.
- **Fail-fast**: Assert liberally to ensure the correctness of internal logic.
    - Use `ASSERT` to check operations that should never fail.
    - Use `DEBUG_ASSERT` for assertions on hot paths that compile away in release.
    - Use `PRECONDITION` to enforce trust boundaries where bad parameters would indicate a logical error.
    - Use `INVARIANT` to enforce internal assumptions.
    - Example: `PRECONDITION(ptr != nullptr, "Pointer cannot be null");`
- **Do not throw exceptions.** Return errors as values.
- Utilize the projects own data structures: `SlotMap`, `SparseSet`, `PagedArray`, ...
- Use descriptive names for template parameters (`Type` not `T`).
- Use `[[nodiscard]]` on anything returning a handle, pointer, or error-bearing type.
- No `auto` for non-trivial types where the type name is load-bearing documentation.
- Use type aliases: `u8/u16/u32/u64`, `i8/i16/i32/i64`, `f32/f64` from `core/types.hh`
- Use `u32` instead of `size_t` to represent indices.
- Use `nil` (equal to `UINT32_MAX`) to represent a null index.
- Containers that are meant to store large amounts of data delete or privatize the copy constructor and expose an explicit `copy()` method to prevent silent expensive copies.
- Naming conventions: Use `snake_case` for functions and variables, `PascalCase` for types, and trailing underscores for private member variables (i.e. `width_`, `first_free_`).

## Game Design
- This project uses an Entity-Component-System (ECS) pattern
- Components represent what an entity *is*, what it *can do*, and what it *is doing*.
- *Components are capabilities*: their presence declares that an entity supports a given interaction. Use this to decide whether something belongs as a component.
- Sort components into two buckets based on lifetime:
    - Long-lived: stable for the entity's lifetime (i.e. Pose)
    - Short-lived: transient, consumed by the action or effect pipeline (i.e. MoveAction)
- Directors generate commands. Commands communicate intent. Commands are translated to actions. Actions are temporary components.
- Determinism allows reproducibility in procedural generation and eventually lockstep multiplayer.
- Content is authored in Lua.
    - During loading, content definitions are compiled into immutable `Codex` entries.
    - Runtime systems operate on integer IDs rather than string identifiers.
    - Lua may be used for configuration, declarative gameplay rules, and cold-path gameplay logic.
    - Hot-path performance-critical simulation code belongs in C++.

## Git Conventions
- Keep the commit title succinct, elaborate in the commit's description.
- Follow the format described by [Conventional Commits](https://www.conventionalcommits.org/en/v1.0.0/).
- Branch naming also follows Conventional Commits vocabulary: `feat/`, `fix/`, `refactor/`

## Constraints
- Simulation systems and procedural generation must remain deterministic.
- Components are known at compile-time. Adding a component type in the middle of a `TypeList` is a breaking change because it changes the indices of components following it; append only.
- `Codex` is effectively immutable after loading — do not mutate definitions mid-simulation.

