# AREG coding rules

Normative rules for code **inside this repository**. Each rule has an identity, a
single sentence, and where it applies. No rationale: the reasoning, the discussion
and the longer examples are in `docs/AREG_CODING_STYLE.md`, which cites these
identities and never restates a rule.

An application built **with** areg is not bound by this file. It follows the API
contract in `docs/agent/20-service-interface.md` and may use any style it likes.

The language level of this repository is C++17. It is a floor for applications, not a
limit: the public headers compile as C++17, C++20 and C++23.

| Band | Meaning |
|---|---|
| **MUST** | A violation is a defect. Fix it. |
| **SHOULD** | Follow it unless the surrounding code already does otherwise. |

---

## N - Naming

| ID | Band | Rule |
|---|---|---|
| N-01 | MUST | Types, classes, structs and enums are `PascalCase`. |
| N-02 | MUST | Functions and methods are `snake_case`. |
| N-03 | MUST | Member variables are `mPascalCase`. |
| N-04 | MUST | Constants and enum values are `UPPER_SNAKE_CASE`, except generated code, which keeps the document's spelling. |
| N-05 | MUST | An accessor is named after what it returns, with no `get_` prefix. |
| N-06 | MUST | A predicate starts with `is_`, `has_` or `can_`. |
| N-07 | MUST | A mutator starts with `set_`. |
| N-08 | SHOULD | A name is clear before it is short. Abbreviate only what the whole repository already abbreviates. |

```cpp
uint32_t get_count() const;      // WRONG  (N-05)
uint32_t count() const;          // RIGHT

bool valid() const;              // WRONG  (N-06)
bool is_valid() const;           // RIGHT
```

Details and the full decision tree: `docs/AREG_CODING_STYLE.md` section 1.

---

## T - Types

| ID | Band | Rule |
|---|---|---|
| T-01 | MUST | Read-only string parameters are `std::string_view`, never `const char *`. |
| T-02 | MUST | Integers are fixed width: `uint32_t`, `int64_t`. Never `unsigned int` or bare `long`. |
| T-03 | MUST | The null pointer is `nullptr`. Never `NULL`, never `0`. |
| T-04 | MUST | Enumerations are `enum class`. Never a plain `enum`. |
| T-05 | SHOULD | A nullable return is `std::optional<T>`. |
| T-06 | SHOULD | Owning pointers are smart pointers; a raw pointer never owns. |

```cpp
void set_name(const char * name);        // WRONG  (T-01)
void set_name(std::string_view name);    // RIGHT
```

---

## L - Layout

| ID | Band | Rule |
|---|---|---|
| L-01 | MUST | Braces are Allman: the opening brace is on its own line. |
| L-02 | MUST | Indentation is 4 spaces. Never a tab. |
| L-03 | MUST | Lines are at most 180 characters; 120 is the target. |
| L-04 | MUST | Continued lists carry the comma at the start of the line. |
| L-05 | MUST | One declaration per line. |
| L-06 | SHOULD | The pointer or reference binds to the type: `const String & name`. |
| L-07 | SHOULD | Guard clauses replace nesting deeper than three levels. |

```cpp
Constructor()
    : mFirst ( 1 )
    , mSecond( 2 )      // leading comma  (L-04)
{
}
```

---

## H - Headers and files

| ID | Band | Rule |
|---|---|---|
| H-01 | MUST | Include guards are traditional and spell the path: `AREG_<PATH>_<FILE>_HPP`. `#pragma once` is forbidden. |
| H-02 | MUST | A header under `private/` is included only from inside its own module. The one exception is `framework/areg/base/private/DebugDefs.hpp`. |
| H-03 | MUST | Platform code lives in `private/posix/` or `private/win32/`, never behind scattered `#ifdef` blocks in shared sources. |
| H-04 | SHOULD | Include order: own header, framework headers, standard library, third party. |

---

## C - Classes

| ID | Band | Rule |
|---|---|---|
| C-01 | MUST | Members are initialized in the constructor initializer list, not by an in-class initializer. |
| C-02 | MUST | An overriding declaration carries both `virtual` and `override`. |
| C-03 | MUST | A polymorphic base has a virtual destructor. |
| C-04 | SHOULD | A locked scope uses an RAII guard, never a manual lock and unlock pair. |
| C-05 | SHOULD | `[[nodiscard]]` marks any function whose result must not be dropped. |
| C-06 | SHOULD | `noexcept` states a contract, so put it only where the function truly cannot fail. |

---

## E - Errors

| ID | Band | Rule |
|---|---|---|
| E-01 | MUST | No exceptions. Nothing throws and nothing catches. |
| E-02 | MUST | Failure is reported by `bool`, `std::optional` or an error code. |

---

## M - Macros

| ID | Band | Rule |
|---|---|---|
| M-01 | MUST | A macro name starts with `AREG_`. |
| M-02 | MUST | Empty marker macros `IN`, `OUT` and `INOUT` are forbidden. |
| M-03 | SHOULD | Prefer `constexpr`, `inline` or a template to a macro. |

---

## A - Encoding

| ID | Band | Rule |
|---|---|---|
| A-01 | MUST | Every non-document file contains only ASCII bytes `0x00-0x7F`. Write `(c)`, `--`, `...` and `->`, never their Unicode forms. |
| A-02 | MUST | Never add a non-ASCII byte when editing a file that already has one, and never convert a file in bulk without being asked. |

Check a file with `python3 tools/check-ascii.py <path>`.

---

## D - Comments and documentation

| ID | Band | Rule |
|---|---|---|
| D-01 | MUST | A comment says **what** the code does, never why it was written, what was measured, or what came before. |
| D-02 | MUST | No reference to a task, ticket, issue, pull request, commit, or any note outside the repository. |
| D-03 | MUST | In a header: a class gets a short statement; a method gets what it does, `\param` and `\return`; a member gets exactly one line. |
| D-04 | MUST | A comment in a source file is at most 2 sentences and at most 2 lines. |
| D-05 | MUST | Delete a comment that repeats the line below it or narrates a step. |
| D-06 | SHOULD | `\note` is only for a constraint the caller must obey: ordering, locking, lifetime, a platform limit. |

```cpp
// Now we take the current time and store it.        <- WRONG  (D-05)
const auto now{ std::chrono::steady_clock::now() };
```

---

## Still to merge

These rules are the ones stated across `CLAUDE.md` and `docs/AREG_CODING_STYLE.md`.
The C++17 pattern catalogue (`if constexpr`, structured bindings, `std::array`,
`std::atomic` flags, comparison operator sets, the inline method pattern) is not yet
folded in and today lives only in the modernization skill. No automated checker
enforces these identities yet; every rule above is checked by review.
