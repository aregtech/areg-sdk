# Copilot Instructions for Areg SDK

## Project

Areg SDK is a C++17 cross-platform IPC middleware framework for distributed, service-oriented systems. It targets Linux, macOS, and Windows.

## Coding Rules (MANDATORY)

**Before writing or modifying any C++ code, follow the rules in `docs/AREG_AI_CODING_RULES.md`.** That file is the authoritative coding standard. Key rules:

### Naming

| Element | Convention | Example |
|---------|-----------|---------|
| Class / Struct / Enum | `PascalCase` | `ServiceManager`, `ConnectionState` |
| Functions / Methods | `snake_case` | `start_service()` |
| Class data members | `mPascalCase` | `mSocketFd`, `mAddress` |
| Struct public fields | `snake_case` | `service_address` |
| Static class members | `s_snake_case` | `s_instance_count` |
| Constants (`constexpr`) | `UPPER_SNAKE_CASE` | `MAX_BUFFER_SIZE` |
| Macros | `AREG_UPPER_SNAKE` | `AREG_NOCOPY` |
| Local variables | `snake_case` | `connection_count` |
| Namespaces | `snake_case` | `areg::ipc` |

### Accessors

- `property()` not `get_property()`: `count()`, `size()`, `state()`
- `is_property()` / `has_property()` for booleans: `is_valid()`, `has_data()`
- `set_property()` for mutators: `set_name()`, `set_timeout()`

### Types

- `std::string_view` for read-only string parameters (not `const char*`)
- Fixed-width integers: `uint32_t`, `int64_t` (not `unsigned int`)
- `nullptr` always (never `NULL` or `0`)
- `auto` for iterators/lambdas, never for return types
- `std::optional` for "may not have value" returns
- No exceptions — use `bool`, `std::optional`, or error codes

### Layout

- Allman braces (opening brace on new line)
- 4 spaces, no tabs
- Leading commas in enums, initializer lists, multi-line parameter lists
- Line length: prefer 120, max 180

### Headers

- Traditional include guards: `AREG_<PATH>_<FILENAME>_HPP` (never `#pragma once`)
- Include order: `GEGlobal.h` first, then Areg headers, then STL, then C, then third-party

### Classes

- Constructor initializer lists with leading commas (no in-class member initializers for classes)
- `virtual` + `override` together in declarations
- Member order: public types, static methods, ctors/dtor, public methods, protected, private, member vars, deleted ops
- `inline` for simple getters/setters, implemented outside class body in same header
- Never `inline virtual`

### Macros

- Prefix with `AREG_`
- Prefer `constexpr`, `inline` functions, templates over macros
- No `IN`/`OUT`/`INOUT` empty macros

### Prohibited

- `#pragma once`, `unsigned int/short/char`, `get_` prefix, `inline virtual`, exceptions, tabs, `auto` return types, `NULL`, `const char*` for strings, macros for constants

## Build

```bash
cmake -B build -DAREG_BUILD_TESTS=ON -DAREG_BUILD_EXAMPLES=ON
cmake --build build -j20
ctest --test-dir build
```

## Full References

- **AI coding rules:** `docs/AREG_AI_CODING_RULES.md`
- **Style guide with rationale:** `docs/AREG_CODING_STYLE.md`
