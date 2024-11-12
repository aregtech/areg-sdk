# Key-Value Data Persistence Syntax in AREG SDK

## Overview

The AREG Framework simplifies data persistence with lightweight Key-Value pairs, stored in plain text. Unlike complex formats like XML or JSON, AREG's [persistence module](./../../framework/areg/persist) is optimized for efficient data handling and quick initialization. The persistence module is not suited for large, complex data structures but is ideal for straightforward configuration storage.

---

## Table of Contents
1. [General Information](#1-general-information)
2. [Syntax Basics](#2-syntax-basics)
3. [Property Key](#3-property-key)
4. [Property Value](#4-property-value)
5. [Examples](#5-examples)

---

## 1. General Information

The **AREG SDK** leverages simple key-value pairs, called *Properties* for data persistence. For example, the **persistence module** parses these pairs from configuration files like [areg.init](./../../framework/areg/resources/areg.init), enabling AREG SDK to handle configuration data without the complexities of structured data formats. This design focuses on simplicity and efficiency for essential configurations.

---

## 2. Syntax Basics

The parser in AREG Framework adheres to specific key-value syntax rules:

- **Format**: `<property key> = <property value>`
- **Comments**: Begin with `#` followed by space and are ignored.
- **End of Assignment**: An optional `;` at the end of the value.
- **Whitespace**: Spaces and tabs are ignored.
- **Key Separator**: Uses `::` to organize the key.
- **Value Delimiter**: Uses `|` for lists (similar to logical OR).

**Example**
```text
log::myapp::scope::scope_one = DEBUG | SCOPE;  # Ends with `;`
log::myapp::scope::scope_two = WARN  | SCOPE   # No `;`
```

- **Keys** (e.g., `log::myapp::scope::scope_one`) identify specific categories and configurations.
- **Values** (e.g., `DEBUG | SCOPE`) define properties and behaviors.

---

## 3. Property Key

**Property Key** format: `section::(module|*)::property[::position]` with `::` separators for organization.

- **section** (required): Defines the configuration category.
- **module** (required): Specifies the executable it applies to, or `*` for all executables.
- **property** (required): Indicates the configuration setting.
- **position** (optional): Specifies sub-categories for further organization.

Each key must include *section*, *property*, and either a specific *module* or `*`. For example:
- `log::myapp::scope::scope_one` defines the `scope` property under `log` for the `myapp` executable, specifically for `scope_one`.

> [!NOTE]
> Using `*` in a key (e.g., `log::*::scope::*`) applies the setting globally across all executables and sub-categories.

---

## 4. Property Value

Property values begin after the `=` symbol and extend until `;`, `#`, or line end. Use `|` to separate values in lists, for instance:

```text
location::*::files = ./config/ | ~/config/ | /usr/bin/;  # Multiple file paths
```

In this case, the `location::*::files` key has a list of paths [`./config/`, `~/config/`, `/usr/bin/`].

---

## 5. Examples

Sample key-value pairs within the AREG SDK that specifies the format of logs:
```text
log::*::layout::enter    = %d: [ %t  %x.%z: Enter --> ]%n
log::*::layout::message  = %d: [ %t  %p >>> ] %m%n
log::*::layout::exit     = %d: [ %t  %x.%z: Exit <-- ]%n
```

These configurations define logging layouts applicable to all executables (`*`), with values specifying various message formats.

---

For more details, refer to the **AREG SDK documentation** on [GitHub](https://github.com/aregtech/areg-sdk).
