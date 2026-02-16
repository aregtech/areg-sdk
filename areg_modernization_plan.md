# Areg SDK — Modernization Plan

**Version:** 2.1
**Date:** February 2026
**Purpose:** Complete reference for renaming types, restructuring namespaces, and modernizing code generation.
**Audience:** Developer (Avetis) and AI agents performing automated refactoring.

---

## Table of Contents

1. [Guiding Principles](#1-guiding-principles)
2. [Namespace Strategy](#2-namespace-strategy)
3. [IE-Prefixed Classes (Interfaces)](#3-ie-prefixed-classes)
4. [TE-Prefixed Classes (Templates)](#4-te-prefixed-classes)
5. [Structs](#5-structs)
6. [Enums](#6-enums)
7. [Namespaces](#7-namespaces)
8. [Typos Fixed](#8-typos-fixed)
9. [Code Generation (.siml)](#9-code-generation)
10. [Container Evaluation](#10-container-evaluation)
11. [Migration Checklist](#11-migration-checklist)
12. [Overall Score](#12-overall-score)

---

## 1. Guiding Principles

These rules govern every rename decision in this document.

| # | Principle | Example |
|---|-----------|---------|
| 1 | **Interfaces get the clean name.** When an interface collides with a concrete class, rename the concrete side. | `ByteBuffer` -> `ByteBuffer` (interface); struct `sByteBuffer` -> `RawByteBuffer` |
| 2 | **No mechanical suffixes.** Never use `Base`, `If`, `Data`, `Kind` as collision-avoidance. Every name must carry domain meaning. | `sLogMessage` -> `LogRecord` (not `LogMessageData`) |
| 3 | **PascalCase for types, snake_case for namespaces.** Per `docs/AREG_AI_CODING_RULES.md`. | `BufferHeader`, `areg::os` |
| 4 | **Always use `areg::` namespace.** Remove `AREG_NAMESPACE=0` option. Framework types always live in `areg::`. | `areg::ResultType` — always |
| 5 | **Generated code belongs to the user's namespace.** Generated service classes reference framework types with `areg::` qualification. No re-exports. | `class HelloProvider : public areg::StubBase` |
| 6 | **Fix all typos in the same pass.** No separate "typo fix" phase. | `BufferHeader` -> `BufferHeader` |
| 7 | **Flat `areg::` namespace for the entire areg library.** All public types live directly in `areg::`. No sub-namespaces for subsystems — directories organize source files, not API. | `areg::String`, `areg::Thread`, `areg::ResultType`, `areg::LogPriority` |
| 8 | **Names must be self-descriptive without namespace context.** Since all types share `areg::`, type names carry enough meaning to avoid ambiguity. | `areg::ConnectionProvider` not `areg::ServiceConnectionProvider` |
| 9 | **Platform types use `os::` namespace + platform suffix.** OS abstraction classes use `areg::os::` namespace with `Posix`/`Win32` suffix in the class name. File name = class name. | `areg::os::MutexPosix` in `posix/MutexPosix.hpp` |
| 10 | **Thread-safe variants use `Concurrent` prefix.** Following Java/C# convention for synchronized container aliases. | `ConcurrentStack`, `ConcurrentResourceMap` |

---

## 2. Namespace Strategy

### 2.1 Framework namespace is always `areg::`

The `AREG_NAMESPACE` compile option is removed. All framework types live in `namespace areg`.
Users who want shorter names use standard C++ mechanisms:

```cpp
// Import specific types
using areg::String;
using areg::ResultType;

// Using-directive in .cpp files only (never in headers)
using namespace areg;
```

### 2.2 Namespace hierarchy (2 namespaces for areg library)

All public types from the `areg` library live in a single flat `areg::` namespace.
Directories organize source files for developers; the namespace organizes the API for users.
This matches Abseil (`absl::` for everything) and Qt (`Qt::` for everything) conventions.

```
areg::                       // ALL areg library types from all directories:
                             //   base/:      String, Thread, Mutex, ByteBuffer, Rect,
                             //               Point, Size, SystemTime, HashMap, ArrayList, etc.
                             //   component/: Component, Proxy, Stub, Timer, ResultType,
                             //               ServiceType, RequestType, ModelState, etc.
                             //   ipc/:       RemoteConnection, ConnectionType, ConnectionState,
                             //               RemoteServiceId, SendPolicy, etc.
                             //   logging/:   LogPriority, LogRecord, ScopeInfo, ConfigKey,
                             //               LayoutTag, LogStatus, Grouping, NodeType, etc.
                             //   persist/:   ConfigKeys, PropertyRecord, etc.
                             //   appbase/:   AppState, EntryTypesEnabling, etc.
areg::os::                   // OS abstraction layer: platform-specific primitives
                             //   MutexPosix, SpinLockWin32, SyncObjectPosix, etc.
                             //   Class names carry platform suffix; only one compiles per platform
                             //   Located in private/posix/ and private/win32/ directories

aregext::                    // Extended library (separate from areg library):
                             //   ServiceOption, SystemServiceState, ConnectionBehavior, etc.
mtrouter::                   // Router application (not in areg::)
logcollector::               // Log collector application (not in areg::)
```

**Why flat `areg::` instead of sub-namespaces:**
- Areg's core design principle is **transport transparency** — threading, IPC, and logging are one unified system, not isolated subsystems. Sub-namespaces contradict this.
- `areg::component::Component` says "component" three times. `areg::Component` says it once.
- Sockets live in `base/`, used by both `ipc/` and `logging/` — sub-namespaces would force an arbitrary assignment.
- Developers type `areg::` and see everything. No guessing which sub-namespace a type belongs to.
- Only 2 namespaces to learn: `areg::` (public API) and `areg::os::` (platform internals).
- The `#include` path already tells you the source directory: `#include "areg/component/Component.hpp"` — the namespace doesn't need to repeat it.

### 2.3 Previous namespace → new location mapping

| Old NE Namespace | Types move to | Rationale |
|---|---|---|
| NEApplication | `areg::` | AppState, EntryTypesEnabling |
| NECommon | `areg::` | Constants, SortOrder, IndexPosition, CookieId, RingOverlap |
| NEMath | `areg::` | Rect, Point, Size, Coord, LargeInteger, CompareResult, DigitSign |
| NEMemory | `areg::` | BufferHeader, RawByteBuffer, BufferType, MessageResult, Align |
| NESocket | `areg::` | SocketAddress, UserData |
| NEString | `areg::` | CharDefs, Radix |
| NEUtilities | `areg::` | SystemTime |
| NEDebug | `areg::` | DebugPriority (private by file location) |
| NESyncTypesIX | `areg::os::` | POSIX sync types (platform suffix on class names) |
| NERegistry | `areg::` | ModelState, ComponentEntry, ServiceEntry |
| NEService | `areg::` | ResultType, ServiceType, RequestType, DataState, etc. |
| NEConnection | `areg::` | Connection constants (private by file location) |
| NERemoteService | `areg::` | RemoteConnection, ConnectionType, RemoteServiceId |
| NELogging | `areg::` | LogPriority, LogRecord, ScopeInfo, LogDataType, etc. |
| NELogOptions | `areg::` | ConfigKey, LayoutTag, LogStatus (private by file location) |
| NEPersistence | `areg::` | ConfigKeys, PropertyRecord |
| NESystemService | `aregext::` | ServiceOption, SystemServiceState (separate library) |
| NELogCollectorSettings | `logcollector::` | App-level, not in areg:: |
| NEMultitargetRouterSettings | `mtrouter::` | App-level, not in areg:: |

### 2.4 Macro changes

| Macro | Old behavior | New behavior |
|-------|-------------|-------------|
| `AREG_NAMESPACE_BEGIN` | Conditional `namespace areg {` or nothing | Always `namespace areg {` |
| `AREG_NAMESPACE_END` | Conditional `}` or nothing | Always `}` |
| `AREG_NAMESPACE_USE` | Conditional `using namespace areg;` or nothing | Always `using namespace areg;` |
| `AREG_NAMESPACE` option | `0` or `1` | **Removed.** Always on. |

Keep the macros themselves — they provide consistent syntax across the codebase and allow future flexibility. Just remove the conditional behavior.

---

## 3. IE-Prefixed Classes

Drop `IE`. Interfaces get the natural name. Collisions resolved on the concrete/struct side.
Platform internals replace the `IX` suffix with `Posix` — the `areg::os::` namespace + platform suffix in the class name communicates platform specificity. File name = class name.
Redundant "Service" prefix dropped where namespace already provides context.

| Old Name                          | New Name                        | Old File                                                         | New File                                                         | Collision Resolution / Notes               |
|-----------------------------------|---------------------------------|------------------------------------------------------------------|------------------------------------------------------------------|--------------------------------------------|
| `ByteBuffer`                    | `ByteBuffer`                    | `areg/base/ByteBuffer.hpp`                                     | `areg/base/ByteBuffer.hpp`                                       | Struct `sByteBuffer` -> `RawByteBuffer`    |
| `ConfigListener`         | `ConfigListener`                | `areg/persist/ConfigListener.hpp`                       | `areg/persist/ConfigListener.hpp`                                |                                            |
| `Cursor`                | `Cursor`                        | `areg/base/Cursor.hpp`                                 | `areg/base/Cursor.hpp`                                           |                                            |
| `DatabaseEngine`                | `DatabaseEngine`                | `areg/persist/DatabaseEngine.hpp`                              | `areg/persist/DatabaseEngine.hpp`                                |                                            |
| `EventConsumer`                 | `EventConsumer`                 | `areg/component/EventConsumer.hpp`                             | `areg/component/EventConsumer.hpp`                               |                                            |
| `EventRouter`                   | `EventRouter`                   | `areg/component/EventRouter.hpp`                               | `areg/component/EventRouter.hpp`                                 |                                            |
| `IOStream`                      | `IOStream`                      | `areg/base/IOStream.hpp`                                       | `areg/base/IOStream.hpp`                                         |                                            |
| `InStream`                      | `InStream`                      | `areg/base/IOStream.hpp`                                       | (same file as IOStream)                                          |                                            |
| `LogLayout`                        | `LogLayout`                     | `areg/logging/private/Layouts.hpp`                               | (same file)                                                      |                                            |
| `LogDatabaseEngine`             | `LogDatabaseEngine`             | `areg/logging/LogDatabaseEngine.hpp`                           | `areg/logging/LogDatabaseEngine.hpp`                             |                                            |
| `NotificationConsumer`     | `NotificationConsumer`          | `areg/component/NotificationEvent.hpp`                           | (same file)                                                      |                                            |
| `OutStream`                     | `OutStream`                     | `areg/base/IOStream.hpp`                                       | (same file as IOStream)                                          |                                            |
| `ProxyEventConsumer`            | `ProxyEventConsumer`            | `areg/component/ProxyEvent.hpp`                                  | (same file)                                                      |                                            |
| `ProxyListener`                 | `ProxyListener`                 | `areg/component/ProxyListener.hpp`                             | `areg/component/ProxyListener.hpp`                               |                                            |
| `QueueListener`                 | `QueueListener`                 | `areg/component/private/QueueListener.hpp`                     | `areg/component/private/QueueListener.hpp`                       |                                            |
| `RemoteEventConsumer`           | `RemoteEventConsumer`           | `areg/component/RemoteEventConsumer.hpp`                       | `areg/component/RemoteEventConsumer.hpp`                         |                                            |
| `RemoteMessageHandler`          | `RemoteMessageHandler`          | `areg/ipc/RemoteMessageHandler.hpp`                            | `areg/ipc/RemoteMessageHandler.hpp`                              |                                            |
| `Lockable`                  | `Lockable`                      | `areg/base/SyncPrimitives.hpp`                                      | (same file)                                                      | Concrete `ResourceLock` keeps its name     |
| `ConnectionConsumer`     | `ConnectionConsumer`            | `areg/ipc/ConnectionConsumer.hpp`                       | `areg/ipc/ConnectionConsumer.hpp`                                | In `areg::`, "Service" is redundant        |
| `ConnectionHandler`      | `ConnectionHandler`             | `aregextend/service/ConnectionHandler.hpp`              | `aregextend/service/ConnectionHandler.hpp`                       | In `aregext::`, "Service" is redundant     |
| `ConnectionProvider`     | `ConnectionProvider`            | `areg/ipc/ConnectionProvider.hpp`                       | `areg/ipc/ConnectionProvider.hpp`                                | In `areg::`, "Service" is redundant        |
| `ServiceEventConsumer`      | `ServiceEventConsumer`          | `areg/ipc/ServiceEventConsumer.hpp`                          | `areg/ipc/ServiceEventConsumer.hpp`                              | Keep "Service" to distinguish from `EventConsumer` |
| `RegistrationConsumer`       | `RegistrationConsumer`          | `areg/ipc/RegistrationConsumer.hpp`                         | `areg/ipc/RegistrationConsumer.hpp`                              | "Registration" is better English than "Register" |
| `RegistrationProvider`       | `RegistrationProvider`          | `areg/ipc/RegistrationProvider.hpp`                         | `areg/ipc/RegistrationProvider.hpp`                              | Same reasoning                             |
| `StubEventConsumer`             | `StubEventConsumer`             | `areg/component/StubEvent.hpp`                                   | (same file)                                                      |                                            |
| `SyncObject`                    | `SyncObject`                    | `areg/base/SyncObject.hpp`                                     | `areg/base/SyncObject.hpp`                                       | Enum `eSyncObject` -> `SyncCategory`       |
| `SyncObjectPosix`              | `SyncObjectPosix`               | `areg/base/private/posix/SyncObjectPosix.hpp`                 | `areg/base/private/posix/SyncObjectPosix.hpp`                    | Platform suffix replaces IX; `areg::os::` namespace |
| `ThreadConsumer`                | `ThreadConsumer`                | `areg/base/ThreadConsumer.hpp`                                 | `areg/base/ThreadConsumer.hpp`                                   |                                            |
| `TimerConsumer`                 | `TimerConsumer`                 | `areg/component/TimerConsumer.hpp`                             | `areg/component/TimerConsumer.hpp`                               |                                            |
| `WaitablePosix`                | `WaitablePosix`                 | `areg/base/private/posix/WaitablePosix.hpp`                   | `areg/base/private/posix/WaitablePosix.hpp`                      | Platform suffix replaces IX; `areg::os::` namespace |
| `WorkerThreadConsumer`          | `WorkerThreadConsumer`          | `areg/component/WorkerThreadConsumer.hpp`                      | `areg/component/WorkerThreadConsumer.hpp`                        |                                            |

**Total: 32 renames**

**Changes from v1.0:** `ServiceConnectionConsumer` -> `ConnectionConsumer`, `ServiceConnectionProvider` -> `ConnectionProvider`, `ServiceConnectionHandler` -> `ConnectionHandler`, `ServiceRegisterConsumer` -> `RegistrationConsumer`, `ServiceRegisterProvider` -> `RegistrationProvider`, `SyncObjectBaseIX` -> `SyncObjectPosix`, `WaitableBaseIX` -> `WaitablePosix`, `GenericObject` -> `ObjectBase`.

### 3.1 POSIX IX-Suffixed Classes (non-IE)

All IX-suffixed classes in `posix/` get the `Posix` suffix. Namespace: `areg::os::`.

| Old Name                  | New Name                  | Old File                              | New File                              |
|---------------------------|---------------------------|---------------------------------------|---------------------------------------|
| `CriticalSectionPosix`      | `CriticalSectionPosix`   | `posix/CriticalSectionPosix.hpp`         | `posix/CriticalSectionPosix.hpp`      |
| `MutexPosix`                | `MutexPosix`             | `posix/MutexPosix.hpp`                   | `posix/MutexPosix.hpp`                |
| `ObjectLockPosix`           | `ObjectLockPosix`        | `posix/MutexPosix.hpp`                   | `posix/MutexPosix.hpp`                |
| `SpinLockPosix`             | `SpinLockPosix`          | `posix/SpinLockPosix.hpp`                | `posix/SpinLockPosix.hpp`             |
| `SpinAutolockPosix`         | `SpinAutolockPosix`      | `posix/SpinLockPosix.hpp`                | `posix/SpinLockPosix.hpp`             |
| `SyncLockAndWaitPosix`      | `SyncLockAndWaitPosix`   | `posix/SyncLockAndWaitPosix.hpp`         | `posix/SyncLockAndWaitPosix.hpp`      |
| `WaitableEventPosix`        | `WaitableEventPosix`     | `posix/WaitableEventPosix.hpp`           | `posix/WaitableEventPosix.hpp`        |
| `WaitableMutexPosix`        | `WaitableMutexPosix`     | `posix/WaitableMutexPosix.hpp`           | `posix/WaitableMutexPosix.hpp`        |
| `WaitableSemaphorePosix`    | `WaitableSemaphorePosix` | `posix/WaitableSemaphorePosix.hpp`       | `posix/WaitableSemaphorePosix.hpp`    |
| `WaitableTimerPosix`        | `WaitableTimerPosix`     | `posix/WaitableTimerPosix.hpp`           | `posix/WaitableTimerPosix.hpp`        |

**Total: 10 additional renames** (beyond the 2 IE-prefixed ones in section 3)

Note: `SpinLockWin32` already follows this convention — no rename needed.

---

## 4. TE-Prefixed Classes

Drop `TE`. Thread-safe variants use `Concurrent` prefix (Java/C# convention) instead of `Lock`/`Nolock`.

| Old Name                          | New Name                        | Old File                                                         | New File                                                         | Notes                                      |
|-----------------------------------|---------------------------------|------------------------------------------------------------------|------------------------------------------------------------------|--------------------------------------------|
| `ArrayList`                     | `ArrayList`                     | `areg/base/ArrayList.hpp`                                      | `areg/base/ArrayList.hpp`                                        |                                            |
| `FixedArray`                    | `FixedArray`                    | `areg/base/FixedArray.hpp`                                     | `areg/base/FixedArray.hpp`                                       |                                            |
| `HashMap`                       | `HashMap`                       | `areg/base/HashMap.hpp`                                        | `areg/base/HashMap.hpp`                                          |                                            |
| `IdHashMap`                     | `IdHashMap`                     | `areg/base/Containers.hpp`                                       | (same file)                                                      |                                            |
| `IdMap`                         | `IdMap`                         | `areg/base/Containers.hpp`                                       | (same file)                                                      |                                            |
| `IntegerHashMap`                | `IntHashMap`                    | `areg/base/Containers.hpp`                                       | (same file)                                                      |                                            |
| `IntegerMap`                    | `IntMap`                        | `areg/base/Containers.hpp`                                       | (same file)                                                      |                                            |
| `LinkedList`                    | `LinkedList`                    | `areg/base/LinkedList.hpp`                                     | `areg/base/LinkedList.hpp`                                       |                                            |
| `ConcurrentResourceListMap`           | `ConcurrentResourceListMap`     | `areg/base/ResourceListMap.hpp`                                | (same file)                                                      | `Lock` -> `Concurrent`                     |
| `ConcurrentResourceMap`               | `ConcurrentResourceMap`         | `areg/base/ResourceMap.hpp`                                    | (same file)                                                      | `Lock` -> `Concurrent`                     |
| `ConcurrentRingStack`                 | `ConcurrentRingStack`           | `areg/base/RingStack.hpp`                                      | (same file)                                                      | `Lock` -> `Concurrent`                     |
| `ConcurrentRuntimeResourceMap`        | `ConcurrentRuntimeResourceMap`  | `areg/base/RuntimeResourceMap.hpp`                             | (same file)                                                      | `Lock` -> `Concurrent`                     |
| `ConcurrentStack`                     | `ConcurrentStack`               | `areg/base/Stack.hpp`                                          | (same file)                                                      | `Lock` -> `Concurrent`                     |
| `OrderedMap`                           | `OrderedMap`                    | `areg/base/OrderedMap.hpp`                                            | `areg/base/OrderedMap.hpp`                                       | Wraps `std::map` (ordered)                 |
| `ResourceListMap`         | `ResourceListMap`               | `areg/base/ResourceListMap.hpp`                                | (same file)                                                      | `Nolock` dropped — unsync is the default   |
| `ResourceMap`             | `ResourceMap`                   | `areg/base/ResourceMap.hpp`                                    | (same file)                                                      | `Nolock` dropped                           |
| `RingStack`               | `RingStack`                     | `areg/base/RingStack.hpp`                                      | (same file)                                                      | `Nolock` dropped (same as base template)   |
| `RuntimeResourceMap`      | `RuntimeResourceMap`            | `areg/base/RuntimeResourceMap.hpp`                             | (same file)                                                      | `Nolock` dropped                           |
| `Stack`                   | `Stack`                         | `areg/base/Stack.hpp`                                          | (same file)                                                      | `Nolock` dropped (same as base template)   |
| `PtrHashMap`                | `PtrHashMap`                    | `areg/base/Containers.hpp`                                       | (same file)                                                      |                                            |
| `PtrMap`                    | `PtrMap`                        | `areg/base/Containers.hpp`                                       | (same file)                                                      |                                            |
| `KeyValuePair`                      | `KeyValuePair`                  | `areg/base/KeyValuePair.hpp`                                       | `areg/base/KeyValuePair.hpp`                                     |                                            |
| `ResourceListMapBase`               | `ResourceListMapBase`           | `areg/base/ResourceListMap.hpp`                                | `areg/base/ResourceListMap.hpp`                                  | Base template; aliases are `ResourceListMap` and `ConcurrentResourceListMap` |
| `ResourceListMapImpl`           | `ResourceListMapImpl`           | `areg/base/TemplateBase.hpp`                                   | (same file)                                                      |                                            |
| `ResourceMapBase`                   | `ResourceMapBase`               | `areg/base/ResourceMap.hpp`                                    | `areg/base/ResourceMap.hpp`                                      | Base template; aliases are `ResourceMap` and `ConcurrentResourceMap` |
| `ResourceMapImpl`               | `ResourceMapImpl`               | `areg/base/TemplateBase.hpp`                                   | (same file)                                                      |                                            |
| `RingStack`                     | `RingStackBase`                 | `areg/base/RingStack.hpp`                                      | `areg/base/RingStack.hpp`                                        | Base template; aliases are `RingStack` and `ConcurrentRingStack` |
| `RuntimeHashMap`                | `RuntimeHashMap`                | `areg/base/RuntimeResourceMap.hpp`                             | (same file)                                                      |                                            |
| `RuntimeResourceMapBase`            | `RuntimeResourceMapBase`        | `areg/base/RuntimeResourceMap.hpp`                             | `areg/base/RuntimeResourceMap.hpp`                               | Base template; aliases are `RuntimeResourceMap` and `ConcurrentRuntimeResourceMap` |
| `SortedLinkedList`              | `SortedLinkedList`              | `areg/base/SortedLinkedList.hpp`                               | `areg/base/SortedLinkedList.hpp`                                 |                                            |
| `StackBase`                         | `StackBase`                     | `areg/base/Stack.hpp`                                          | `areg/base/Stack.hpp`                                            | Base template; aliases are `Stack` and `ConcurrentStack` |
| `StringBase`                        | `StringTemplate`                | `areg/base/StringBase.hpp`                                         | `areg/base/StringTemplate.hpp`                                   | `String`/`WideString` inherit from this    |
| `StringHashMap`                 | `StringHashMap`                 | `areg/base/Containers.hpp`                                       | (same file)                                                      |                                            |
| `StringMap`                     | `StringMap`                     | `areg/base/Containers.hpp`                                       | (same file)                                                      |                                            |
| `Align`                         | `Align`                         | `areg/base/MemoryDefs.hpp`                                         | (stays in `areg::` namespace)                                    |                                            |

**Total: 35 renames**

**Changes from v1.0:** `Lock*` -> `Concurrent*`, `Nolock*` -> clean name (unsync is default), base templates that have Concurrent/plain aliases get `Base` suffix to distinguish from the alias.

### Lock/Nolock → Concurrent pattern explained

The base template has a lock policy parameter. The aliases provide convenient names:

```cpp
// Base template (internal — users rarely use directly)
template<typename KEY, typename VALUE, typename Lock = NoLock>
class ResourceMapBase { ... };

// User-facing aliases
using ResourceMap           = ResourceMapBase<KEY, VALUE, NoLock>;      // Unsynchronized (default)
using ConcurrentResourceMap = ResourceMapBase<KEY, VALUE, MutexLock>;   // Thread-safe
```

This is the same pattern as `std::map` vs `std::pmr::map` — a policy-based template with convenient aliases.

---

## 5. Structs

Drop `s`/`S_` prefixes. Domain-specific qualifiers for collisions.
Redundant "Service" prefix dropped where applicable.

| Old Name (inner / alias)                         | New Name                   | File                                              | Collision Resolution                       |
|--------------------------------------------------|----------------------------|---------------------------------------------------|--------------------------------------------|
| `BufferHeader` / `BufferHeader`             | `BufferHeader`             | `areg/base/MemoryDefs.hpp`                          | Fix typo Bufer -> Buffer                   |
| `S_ByteBuffer` / `sByteBuffer`                   | `RawByteBuffer`            | `areg/base/MemoryDefs.hpp`                          | `ByteBuffer` is the interface class        |
| `S_RemoteMessage` / `sRemoteMessage`             | `MessageFrame`             | `areg/base/MemoryDefs.hpp`                          | Wire format = "frame"; context already implies remote |
| `S_RemoteMessageHeader` / `sRemoteMessageHeader` | `MessageHeader`            | `areg/base/MemoryDefs.hpp`                          | Context already implies remote             |
| `S_Rect` / `sRect`                               | `Rect`                     | `areg/base/MathDefs.hpp`                            |                                            |
| `S_Size` / `sSize`                               | `Size`                     | `areg/base/MathDefs.hpp`                            |                                            |
| `S_Point` / `sPoint`                             | `Point`                    | `areg/base/MathDefs.hpp`                            |                                            |
| `S_Coord` / `sCoord`                             | `Coord`                    | `areg/base/MathDefs.hpp`                            |                                            |
| `S_SystemTime` / `sSystemTime`                   | `SystemTime`               | `areg/base/UtilityDefs.hpp`                       |                                            |
| `S_InterfaceData` / `SInterfaceData`             | `InterfaceDescriptor`      | `areg/component/ServiceDefs.hpp`                    | Describes a service interface               |
| `sLargeInteger`                                  | `LargeInteger`             | `areg/base/MathDefs.hpp`                            |                                            |
| `sEntryTypesEnabling`                            | `EntryTypesEnabling`       | `areg/appbase/AppDefs.hpp`                  |                                            |
| `sLogInstance`                                   | `LogInstance`              | `areglogger/client/LogObserverApi.h`              |                                            |
| `sLogMessage`                                    | `LogRecord`                | `areg/logging/LoggingDefs.hpp`                      | Wire format; `LogMessage` is the class     |
| `sLogScope`                                      | `LogScopeEntry`            | `areglogger/client/LogObserverApi.h`              | `LogScope` is the class                    |
| `sObserverEvents`                                | `ObserverEvents`           | `areglogger/client/LogObserverApi.h`              |                                            |
| `sObserverStatus`                                | `ObserverStatus`           | `logobserver/app/LogObserver.hpp`                 |                                            |
| `sOption`                                        | `Option`                   | `aregextend/console/OptionParser.hpp`             |                                            |
| `sOptionSetup`                                   | `OptionSetup`              | `aregextend/console/OptionParser.hpp`             |                                            |
| `sProperty`                                      | `PropertyRecord`           | `areg/persist/PersistenceDefs.hpp`                  | Serialized form; `Property` is the class   |
| `sPropertyKey`                                   | `PropertyKeyRecord`        | `areg/persist/PersistenceDefs.hpp`                  | Same reasoning                             |
| `sScopeFilter`                                   | `ScopeFilter`              | `aregextend/db/LogSqliteDatabase.hpp`             |                                            |
| `sScopeInfo`                                     | `ScopeInfo`                | `areg/logging/LoggingDefs.hpp`                      |                                            |
| `sServiceConnectedInstance`                      | `ConnectedInstance`        | `areg/component/ServiceDefs.hpp`                    | In `areg::`, "Service" is redundant            |
| `BufferAllocator`                                | (keep)                     | `areg/base/MemoryDefs.hpp`                          |                                            |
| `BufferDeleter`                                  | (keep)                     | `areg/base/MemoryDefs.hpp`                          |                                            |
| `Range`                                          | (keep)                     | `aregextend/console/OptionParser.hpp`             |                                            |

**Total: 24 renames, 3 kept**

**Changes from v1.0:** `RemoteMessageFrame` -> `MessageFrame`, `RemoteMessageHeader` -> `MessageHeader`, `ServiceConnectedInstance` -> `ConnectedInstance` (namespace provides context).

---

## 6. Enums

Drop `e`/`E_` prefixes. PascalCase. Merge typedef inner/alias pairs.
Redundant "Service" prefix dropped where namespace or context already implies it.

| Old Name (inner / alias)                         | New Name                   | Namespace          | File                                              | Notes                                      |
|--------------------------------------------------|----------------------------|--------------------|---------------------------------------------------|--------------------------------------------|
| `E_AppState` / `eApplicationState`               | `AppState`                 | `areg::`           | `areg/appbase/AppDefs.hpp`                  |                                            |
| `E_CharDefs` / `eCharDefs`                       | `CharDefs`                 | `areg::`           | `areg/base/StringDefs.hpp`                          |                                            |
| `E_CompareResult` / `eCompare`                   | `CompareResult`            | `areg::`           | `areg/base/MathDefs.hpp`                            |                                            |
| `E_CompletionStatus` / `eCompletionStatus`       | `CompletionStatus`         | `areg::`           | `areg/base/Thread.hpp`                            |                                            |
| `E_ConfigKey` / `eConfigKey`                     | `ConfigKey`                | `areg::`      | `areg/logging/private/LogOptions.hpp`           |                                            |
| `E_ConnectionBehavior` / `eConnectionBehavior`   | `ConnectionBehavior`       | `aregext::`      | `aregextend/service/ServiceCommunicationBase.hpp`  |                                            |
| `E_Cookies` / `eCookies`                         | `CookieId`                 | `areg::`           | `areg/base/CommonDefs.hpp`                          |                                            |
| `E_CursorPosition` / `eCursorPosition`           | `SeekOrigin`               | `areg::`           | `areg/base/Cursor.hpp`                  | Matches POSIX/C# convention                |
| `E_DataStateType` / `eDataStateType`             | `DataState`                | `areg::`| `areg/component/ServiceDefs.hpp`                    |                                            |
| `E_DegubPrio` / `DebugPriority`                     | `DebugPriority`            | `areg::`   | `areg/base/private/DebugDefs.hpp`                   | Fix typo                                   |
| `E_DigitSign` / `eDigitSign`                     | `DigitSign`                | `areg::`           | `areg/base/MathDefs.hpp`                            |                                            |
| `E_DisconnectReason` / `eDisconnectReason`       | `DisconnectReason`         | `areg::`| `areg/component/ServiceDefs.hpp`                    |                                            |
| `E_EventData` / `eEventData`                     | `EventPayload`             | `areg::`| `areg/component/EventDataStream.hpp`              | `EventData` is a class                     |
| `E_EventFired` / `eEventFired`                   | `EventFired`               | `areg::os::` | `areg/base/private/posix/SyncLockAndWaitPosix.hpp` |                                          |
| `E_EventOrder` / `eEventOrder`                   | `EventOrder`               | `areg::`| `areg/component/private/EventDispatcherBase.hpp`  |                                            |
| `E_EventPriority` / `eEventPriority`             | `EventPriority`            | `areg::`| `areg/component/Event.hpp`                        |                                            |
| `E_EventResetInfo` / `eEventResetInfo`           | `EventResetInfo`           | `areg::os::` | `areg/base/private/posix/SyncDefsPosix.hpp` |                                            |
| `E_EventType` / `eEventType`                     | `EventType`                | `areg::`| `areg/component/Event.hpp`                        |                                            |
| `E_ExitCodes` / `eExitCodes`                     | `ExitCode`                 | `areg::`           | `areg/base/ThreadConsumer.hpp`                  | Singular                                   |
| `E_FileOpenBits` / `eFileOpenBits`               | `FileOpenBits`             | `areg::`           | `areg/base/FileBase.hpp`                          |                                            |
| `E_FileOpenMode` / `eFileOpenMode`               | `FileOpenMode`             | `areg::`           | `areg/base/FileBase.hpp`                          |                                            |
| `E_FuncIdRange` / `eFuncIdRange`                 | `FuncIdRange`              | `areg::`| `areg/component/ServiceDefs.hpp`                    |                                            |
| `E_InstanceBitness` / `eInstanceBitness`         | `InstanceBitness`          | `areg::`| `areg/component/ServiceDefs.hpp`                    |                                            |
| `E_Layouts` / `eLayouts`                         | `LayoutTag`                | `areg::`      | `areg/logging/private/LogOptions.hpp`           |                                            |
| `E_LogStatus` / `eLogStatus`                     | `LogStatus`                | `areg::`      | `areg/logging/private/LogOptions.hpp`           |                                            |
| `E_LoggingAction` / `eLoggingAction`             | `LoggingAction`            | `areg::`      | `areg/logging/private/LoggingEvent.hpp`           |                                            |
| `E_MatchCondition` / `eMatchCondition`           | `MatchCondition`           | `areg::os::` | `areg/base/private/posix/SyncDefsPosix.hpp` |                                            |
| `E_MessageDataType` / `eMessageDataType`         | `MessageDataType`          | `areg::`| `areg/component/ServiceDefs.hpp`                    |                                            |
| `E_MessageSource` / `eMessageSource`             | `MessageSource`            | `areg::`| `areg/component/ServiceDefs.hpp`                    |                                            |
| `E_ProcEnv` / `eProcEnv`                         | `ProcessEnv`               | `areg::`           | `areg/base/Process.hpp`                           |                                            |
| `E_Radix` / `eRadix`                             | `Radix`                    | `areg::`           | `areg/base/StringDefs.hpp`                          |                                            |
| `E_RequestType` / `eRequestType`                 | `RequestType`              | `areg::`| `areg/component/ServiceDefs.hpp`                    |                                            |
| `E_ServiceCall` / `eServiceCalls`                | `ServiceCall`              | `areg::`| `areg/component/ServiceDefs.hpp`                    | Singular                                   |
| `E_ServiceConnection` / `eServiceConnection`     | `ConnectionStatus`         | `areg::`| `areg/component/ServiceDefs.hpp`                    | "Service" dropped; "Status" more precise than "Connection" as enum |
| `E_ServiceControl` / `eServiceControl`           | `ServiceControl`           | `aregext::`      | `aregextend/service/SystemServiceBase.hpp`        |                                            |
| `E_ServiceManagerCommands` / `eServiceManagerCommands` | `ManagerCommand`    | `areg::`| `areg/component/private/ServiceManagerEvents.hpp` | Singular; in component::, "Service" redundant |
| `E_ServiceRequestType` / `eServiceRequestType`   | `ServiceRequestType`       | `areg::`| `areg/component/ServiceDefs.hpp`                    |                                            |
| `E_ServiceType` / `eServiceType`                 | `ServiceType`              | `areg::`| `areg/component/ServiceDefs.hpp`                    |                                            |
| `E_SpecialFolder` / `eSpecialFolder`             | `SpecialFolder`            | `areg::`           | `areg/base/File.hpp`                              |                                            |
| `E_SyncObject` / `eSyncObject`                   | `SyncCategory`             | `areg::`           | `areg/base/SyncObject.hpp`                      | `SyncObject` taken by class                |
| `E_SyncObjectFired` / `eSyncObjectFired`         | `SyncObjectFired`          | `areg::os::` | `areg/base/private/posix/SyncDefsPosix.hpp` |                                            |
| `E_ThreadPriority` / `eThreadPriority`           | `ThreadPriority`           | `areg::`           | `areg/base/Thread.hpp`                            |                                            |
| `E_WaitType` / `eWaitType`                       | `WaitType`                 | `areg::os::` | `areg/base/private/posix/SyncLockAndWaitPosix.hpp` |                                          |
| `ResultType`                                     | (keep)                     | `areg::`| `areg/component/ServiceDefs.hpp`                    |                                            |
| `eBufferType`                                    | `BufferType`               | `areg::`           | `areg/base/MemoryDefs.hpp`                          |                                            |
| `eColumnType`                                    | `ColumnType`               | `aregext::`      | `aregextend/db/SqliteStatement.hpp`               |                                            |
| `eConfigKeys`                                    | `ConfigKeys`               | `areg::`  | `areg/persist/PersistenceDefs.hpp`                  |                                            |
| `eConnectionState`                               | `ConnectionState`          | `areg::`      | `areg/ipc/ServiceClientConnectionBase.hpp`        |                                            |
| `eConnectionTypes`                               | `ConnectionType`           | `areg::`      | `areg/ipc/RemoteServiceDefs.hpp`                    | Singular                                   |
| `Grouping`                                     | `Grouping`                 | `areg::`      | `areg/logging/private/ScopeNodeBase.hpp`          | Fix typo                                   |
| `eIndex`                                         | `IndexPosition`            | `areg::`           | `areg/base/CommonDefs.hpp`                          |                                            |
| `eLockedState`                                   | `LockedState`              | `areg::`           | `areg/base/SyncPrimitives.hpp`                       |                                            |
| `eLogDataType`                                   | `LogDataType`              | `areg::`      | `areg/logging/LoggingDefs.hpp`                      |                                            |
| `eLogMessageType`                                | `LogMessageType`           | `areg::`      | `areg/logging/LoggingDefs.hpp`                      |                                            |
| `eLogPriority`                                   | `LogPriority`              | `areg::`      | `areg/logging/LoggingDefs.hpp`                      |                                            |
| `eLogType`                                       | `LogType`                  | `areg::`      | `areglogger/client/LogObserverApi.h`              |                                            |
| `eLoggerOptions`                                 | `LoggerOption`             | `logcollector::`   | `logcollector/app/LogCollector.hpp`               | Singular                                   |
| `LoggingType`                                   | `LoggingType`              | `areg::`      | `areg/logging/LoggingDefs.hpp`                      | Fix typo, singular                         |
| `eMessageResult`                                 | `MessageResult`            | `areg::`           | `areg/base/MemoryDefs.hpp`                          |                                            |
| `eModelState`                                    | `ModelState`               | `areg::`| `areg/component/Model.hpp`                   |                                            |
| `eNode`                                          | `NodeType`                 | `areg::`      | `areg/logging/private/ScopeNodeBase.hpp`          |                                            |
| `eObserverStates`                                | `ObserverState`            | `areg::`      | `areglogger/client/LogObserverApi.h`              | Singular                                   |
| `eQueryResult`                                   | `QueryResult`              | `aregext::`      | `aregextend/db/SqliteStatement.hpp`               |                                            |
| `eRemoteConnection`                              | `RemoteConnection`         | `areg::`      | `areg/ipc/RemoteServiceDefs.hpp`                    |                                            |
| `eRemoteServices`                                | `RemoteServiceId`          | `areg::`      | `areg/ipc/RemoteServiceDefs.hpp`                    |                                            |
| `eRingOverlap`                                   | `RingOverlap`              | `areg::`           | `areg/base/CommonDefs.hpp`                          |                                            |
| `eRouterOptions`                                 | `RouterOption`             | `mtrouter::`       | `mtrouter/app/MultitargetRouter.hpp`              | Singular                                   |
| `eSendMessage`                                   | `SendPolicy`               | `areg::`      | `areg/ipc/SendMessageEvent.hpp`                   |                                            |
| `eServiceEventCommands`                          | `EventCommand`             | `areg::`      | `areg/ipc/ServiceEvent.hpp`                       | Singular; "Service" dropped (in net::)     |
| `eServiceOption`                                 | `ServiceOption`            | `aregext::`      | `aregextend/service/SystemServiceDefs.hpp`          |                                            |
| `eSort`                                          | `SortOrder`                | `areg::`           | `areg/base/CommonDefs.hpp`                          |                                            |
| `eSystemServiceState`                            | `SystemServiceState`       | `aregext::`      | `aregextend/service/SystemServiceDefs.hpp`          |                                            |
| `eTimerType`                                     | `TimerType`                | `areg::`| `areg/component/TimerBase.hpp`                    |                                            |
| `eValidFlags`                                    | `ValidFlags`               | `aregext::`      | `aregextend/console/OptionParser.hpp`             |                                            |
| `eWaitResult`                                    | `WaitResult`               | `areg::`           | `areg/base/SyncPrimitives.hpp`                       |                                            |

**Total: ~90 renames (deduplicated pairs), 1 kept**

**Changes from v1.0:** Added namespace column for clarity. `ServiceConnection` -> `ConnectionStatus` (more precise). `ServiceManagerCommand` -> `ManagerCommand`. `ServiceEventCommand` -> `EventCommand`. All POSIX internals explicitly in `areg::os::`.

**Changes in v2.1:** All sub-namespaces (`areg::component::`, `areg::net::`, `areg::log::`, `areg::persist::`) collapsed into flat `areg::`. `areg::ext::` → `aregext::` (separate library). Directories organize source files, not API.

---

## 7. Namespaces

File renames — the NE prefix is dropped. The C++ namespace identifier changes per section 2.3.

**File naming convention:**
- Class files: `PascalCase.hpp` (e.g., `Component.hpp`, `Thread.hpp`)
- Definition collection files (ex-NE namespaces): `PascalCaseDefs.hpp` (e.g., `MathDefs.hpp`, `ServiceDefs.hpp`)
- Exception: `NERegistry.hpp` → `Model.hpp` — contains actual classes centered around the `Model` class

The `Defs` suffix instantly signals "not a class" while keeping PascalCase consistent with the rest of the codebase.

| Old NE Namespace | New Namespace | Old Primary File | New Primary File |
|---|---|---|---|
| `NEApplication` | `areg::` | `areg/appbase/AppDefs.hpp` | `areg/appbase/AppDefs.hpp` |
| `NECommon` | `areg::` | `areg/base/CommonDefs.hpp` | `areg/base/CommonDefs.hpp` |
| `NEMath` | `areg::` | `areg/base/MathDefs.hpp` | `areg/base/MathDefs.hpp` |
| `NEMemory` | `areg::` | `areg/base/MemoryDefs.hpp` | `areg/base/MemoryDefs.hpp` |
| `NESocket` | `areg::` | `areg/base/SocketDefs.hpp` | `areg/base/SocketDefs.hpp` |
| `NEString` | `areg::` | `areg/base/StringDefs.hpp` | `areg/base/StringDefs.hpp` |
| `NEUtilities` | `areg::` | `areg/base/UtilityDefs.hpp` | `areg/base/UtilityDefs.hpp` |
| `NEDebug` | `areg::` | `areg/base/private/DebugDefs.hpp` | `areg/base/private/DebugDefs.hpp` |
| `NESyncTypesIX` | `areg::os::` | `areg/base/private/posix/SyncDefsPosix.hpp` | `areg/base/private/posix/SyncDefsPosix.hpp` |
| `NERegistry` | `areg::` | `areg/component/Model.hpp` | `areg/component/Model.hpp` |
| `NEService` | `areg::` | `areg/component/ServiceDefs.hpp` | `areg/component/ServiceDefs.hpp` |
| `NEConnection` | `areg::` | `areg/ipc/private/ConnectionDefs.hpp` | `areg/ipc/private/ConnectionDefs.hpp` |
| `NERemoteService` | `areg::` | `areg/ipc/RemoteServiceDefs.hpp` | `areg/ipc/RemoteServiceDefs.hpp` |
| `NELogging` | `areg::` | `areg/logging/LoggingDefs.hpp` | `areg/logging/LoggingDefs.hpp` |
| `NELogOptions` | `areg::` | `areg/logging/private/LogOptions.hpp` | `areg/logging/private/LogOptions.hpp` |
| `NEPersistence` | `areg::` | `areg/persist/PersistenceDefs.hpp` | `areg/persist/PersistenceDefs.hpp` |
| `NESystemService` | `aregext::` | `aregextend/service/SystemServiceDefs.hpp` | `aregextend/service/SystemServiceDefs.hpp` |
| `NELogCollectorSettings` | `logcollector::` | `logcollector/app/LogCollectorNames.hpp` | `logcollector/app/LogCollectorNames.hpp` |
| `NEMultitargetRouterSettings` | `mtrouter::` | `mtrouter/app/MTRouterNames.hpp` | `mtrouter/app/MTRouterNames.hpp` |

**Total: 19 renames**

**Changes from v1.0:** `Defs` suffix convention for ex-namespace files. `NERegistry.hpp` → `Model.hpp` (contains classes, not just definitions). 17 sub-namespaces collapsed into 2: flat `areg::` + `areg::os::`. No file name conflicts with existing class files.

---

## 8. Typos Fixed

These are corrected as part of the renames above. Listed here for tracking.

| Old (typo)                  | Corrected                      | File                                              |
|-----------------------------|--------------------------------|---------------------------------------------------|
| `BufferHeader`              | `BufferHeader`                 | `areg/base/MemoryDefs.hpp`                          |
| `BufferHeader`         | `BufferHeader`                 | `areg/base/MemoryDefs.hpp`                          |
| `DayTimeLayout`            | `DayTimeLayout`                | `areg/logging/private/Layouts.hpp`                |
| `Grouping`                | `Grouping`                     | `areg/logging/private/ScopeNodeBase.hpp`          |
| `LoggingType`              | `LoggingType`                  | `areg/logging/LoggingDefs.hpp`                      |
| `DebugPriority`                | `DebugPriority`                | `areg/base/private/DebugDefs.hpp`                   |
| `ServiceCommunicationBase`   | `ServiceCommunicationBase`     | `aregextend/service/ServiceCommunicationBase.hpp`  |

**Total: 7 typos**

---

## 9. Code Generation

### 9.1 Always `areg::` namespace

Generated code references framework types with full `areg::` qualification:

```cpp
class HelloServiceProvider : public areg::StubBase
{
    virtual void response_hello(areg::ResultType result) = 0;
};
```

No re-exports. No conditional `#if`. No `AREG_NS` macro. Clean and deterministic.

### 9.2 `.siml` new attributes

```xml
<ServiceInterface FormatVersion="2.0"
                  Name="HelloService"
                  Namespace="hello_service"
                  Version="1.0.0">
    <!-- service definition -->
</ServiceInterface>
```

| Attribute   | Required | Default                                  | Rule                                               |
|-------------|----------|------------------------------------------|----------------------------------------------------|
| `Name`      | Yes      | —                                        | PascalCase. Used for generated class names.        |
| `Namespace` | No       | Auto-derived: `Name` -> `snake_case`     | snake_case. Shared namespace for types/constants.  |
| `Version`   | No       | `"1.0.0"`                                | Semantic version.                                  |

**Auto-derivation rule for `Namespace`:**

```
PascalCase Name    →    snake_case Namespace
─────────────────────────────────────────────
HelloService       →    hello_service
IPCRouter          →    ipc_router
DBLogger           →    db_logger
```

Developer can override: `Namespace="hello"` or `Namespace="my_custom_ns"`.

### 9.3 Generated files (10 files from one .siml)

Example: `HelloService.siml` with `Name="HelloService"`, `Namespace="hello_service"`

| # | Old File                       | New File                        | Purpose                                            |
|---|--------------------------------|---------------------------------|----------------------------------------------------|
| 1 | `NEHelloService.hpp`           | `hello_service.hpp`             | Shared namespace: types, constants, message IDs, includes |
| 2 | `NEHelloService.cpp`           | `hello_service.cpp`             | String tables, type serialization                  |
| 3 | `HelloServiceEvents.hpp`       | `HelloServiceEvents.hpp`        | Request/response/notification event classes         |
| 4 | `HelloServiceEvents.cpp`       | `HelloServiceEvents.cpp`        | Event implementation                               |
| 5 | `HelloServiceStub.hpp`         | `HelloServiceProvider.hpp`      | Service provider base class                        |
| 6 | `HelloServiceStub.cpp`         | `HelloServiceProvider.cpp`      | Dispatch, serialization                            |
| 7 | `HelloServiceProxy.hpp`        | `HelloServiceProxy.hpp`         | Proxy class (unchanged name)                       |
| 8 | `HelloServiceProxy.cpp`        | `HelloServiceProxy.cpp`         | Request forwarding, attribute caching              |
| 9 | `HelloServiceClientBase.hpp`   | `HelloServiceConsumer.hpp`      | Service consumer base class                        |
| 10| `HelloServiceClientBase.cpp`   | `HelloServiceConsumer.cpp`      | Notification routing                               |

### 9.4 Generated class names

| Role                 | Old Pattern          | New Pattern              | Example                        |
|----------------------|----------------------|--------------------------|--------------------------------|
| Shared namespace     | `NEXxx`              | `xxx_namespace`          | `hello_service::`              |
| Provider (server)    | `XxxStub`            | `XxxProvider`            | `HelloServiceProvider`         |
| Proxy (client-side)  | `XxxProxy`           | `XxxProxy`               | `HelloServiceProxy`            |
| Consumer (client)    | `XxxClientBase`      | `XxxConsumer`            | `HelloServiceConsumer`         |

### 9.5 How it reads for the developer

```cpp
// hello_service.hpp (generated shared namespace)
namespace hello_service {

    enum class MessageId : uint32_t
    {
          RequestHello    = 0
        , ResponseHello   = 1
    };

    constexpr uint32_t DEFAULT_TIMEOUT { 5000 };

    struct HelloData
    {
        areg::String    name;
        uint32_t        count { 0 };
    };

} // namespace hello_service
```

```cpp
// Developer implements provider
#include "generated/HelloServiceProvider.hpp"

class HelloService : public HelloServiceProvider
{
    void request_hello(const areg::String& name) override
    {
        hello_service::HelloData data;
        data.name  = name;
        data.count = 1;
        response_hello(areg::ResultType::Success);
    }
};
```

```cpp
// Developer implements consumer
#include "generated/HelloServiceConsumer.hpp"

class HelloClient : public HelloServiceConsumer
{
    void on_hello_response(areg::ResultType result) override
    {
        if (result == areg::ResultType::Success)
        {
            // handle success
        }
    }
};
```

---

## 10. Container Evaluation

**Status: Deferred.** The containers are renamed (section 4) but their architecture is evaluated separately.

### 10.1 Container categories

Based on analysis of each container's implementation:

| Category | Containers | Verdict |
|----------|-----------|---------|
| **Genuinely unique** | `RingStack` (circular buffer with overlap modes), `ResourceMap`/`ResourceListMap` (resource lifecycle + locking), `SortedLinkedList` (auto-sorted insertion) | **Keep.** No STL equivalent provides these semantics. |
| **Thin wrappers with Areg API** | `ArrayList` (std::vector), `LinkedList` (std::list), `HashMap` (std::unordered_map), `OrderedMap` (std::map), `Stack` (std::deque) | **Evaluate later.** These add streaming support, bounds checking, and position-based API. Whether the added value justifies the wrapper is a separate decision. |
| **Convenience aliases** | `IdHashMap`, `IntHashMap`, `StringHashMap`, `PtrHashMap`, `IdMap`, `IntMap`, `StringMap`, `PtrMap` | **Keep as type aliases.** Zero cost, improve readability. |
| **Minimal value** | `FixedArray` (manual memory, not std::array), `KeyValuePair` (named pair) | **Evaluate later.** May be replaceable with `std::array` or structured bindings. |

### 10.2 Future direction (not part of current refactoring)

If thin wrappers are removed in a future version:
- Framework API would use `std::vector<T>`, `std::unordered_map<K,V>`, etc. directly
- Streaming support would move to free functions or ADL-based `operator<<`/`operator>>`
- This would be a **major version** change (API-breaking)

---

## 11. Migration Checklist

Ordered by dependency — complete each phase before starting the next.

### Phase 1: Namespace infrastructure
- [ ] Remove `AREG_NAMESPACE` conditional from `AREG_NAMESPACE_BEGIN/END/USE` macros
- [ ] Make `areg::` namespace always active
- [ ] Collapse all 17 NE sub-namespaces into flat `areg::` (per section 2.2)
- [ ] Move all areg library types (base/, component/, ipc/, logging/, persist/) into flat `areg::`
- [ ] Move platform-specific types into `areg::os::`
- [ ] Move aregextend/ types into `aregext::`
- [ ] Rename 19 namespace files (drop NE prefix, use PascalCase)
- [ ] Update all `#include` paths
- [ ] Update `areg.init` configuration file if it references old namespace names
- [ ] Build and fix

### Phase 2: Interface classes (IE)
- [ ] Rename 32 IE-prefixed classes (per section 3)
- [ ] Rename IX-suffixed POSIX classes to platform suffix (MutexPosix, SyncObjectPosix, WaitablePosix, etc.)
- [ ] Shorten verbose names (ConnectionConsumer, ConnectionProvider, etc.)
- [ ] Rename corresponding files
- [ ] Update all `#include` directives
- [ ] Update all inheritance declarations
- [ ] Build and fix

### Phase 3: Template classes (TE)
- [ ] Rename 35 TE-prefixed classes (per section 4)
- [ ] Rename Lock* to Concurrent*, drop Nolock* prefix
- [ ] Add Base suffix to parameterized templates that have Concurrent/plain aliases
- [ ] Rename corresponding files
- [ ] Update all `#include` directives
- [ ] Update all template instantiations
- [ ] Build and fix

### Phase 4: Structs
- [ ] Rename 24 structs (drop `s`/`S_` prefixes, per section 5)
- [ ] Shorten verbose names (MessageFrame, MessageHeader, ConnectedInstance)
- [ ] Remove typedef pairs (keep only the new PascalCase name)
- [ ] Update all usage sites
- [ ] Build and fix

### Phase 5: Enums
- [ ] Rename ~90 enums (drop `e`/`E_` prefixes, per section 6)
- [ ] Shorten verbose names (ConnectionStatus, ManagerCommand, EventCommand)
- [ ] Remove typedef pairs (keep only the new PascalCase name)
- [ ] Update all usage sites
- [ ] Build and fix

### Phase 6: Code generator
- [ ] Add `Namespace` attribute to `.siml` schema
- [ ] Update code generator to emit new file names and class names
- [ ] Update code generator to use `areg::ResultType` (new namespace path)
- [ ] Rename generated classes: `XxxStub` -> `XxxProvider`, `XxxClientBase` -> `XxxConsumer`
- [ ] Rename generated namespace files: `NEXxx` -> `xxx_namespace`
- [ ] Update all examples (01-29) to use new generated names
- [ ] Regenerate all example service interfaces
- [ ] Build and test all examples

### Phase 7: Cleanup
- [ ] Fix all 7 typos listed in section 8
- [ ] Update documentation (`docs/wiki/`, `README.md`, `CONTRIBUTING.md`)
- [ ] Update `CLAUDE.md` and `.github/copilot-instructions.md`
- [ ] Update `docs/AREG_AI_CODING_RULES.md` and `docs/AREG_CODING_STYLE.md`
- [ ] Add `.clang-tidy` configuration (deferred until refactoring complete)
- [ ] Run full test suite
- [ ] Tag release

### Phase 8: Container evaluation (future, separate release)
- [ ] Profile which thin wrapper containers are used in public API vs internal only
- [ ] Decide per-container: keep wrapper, replace with std::, or deprecate
- [ ] If removing wrappers: migrate streaming to free functions
- [ ] Major version bump if API-breaking changes

---

## 12. Overall Score

| Category                          | v1.0   | v2.0   | What changed                                       |
|-----------------------------------|--------|--------|----------------------------------------------------|
| Type naming (classes)             | 9.5/10 | 10/10  | Verbose names shortened, IX suffix removed         |
| Type naming (structs)             | 9.0/10 | 9.5/10 | Redundant "Remote"/"Service" dropped               |
| Type naming (enums)               | 9.5/10 | 10/10  | Namespace column clarifies placement; redundant prefixes dropped |
| Type naming (templates)           | 9.5/10 | 10/10  | Lock/Nolock -> Concurrent/default; standard convention |
| Namespace naming                  | 9.0/10 | 10/10  | Flat `areg::` for entire library; matches Abseil/Qt |
| Namespace architecture            | 9.5/10 | 10/10  | 2 namespaces total (areg:: + areg::os::); directories organize source, not API |
| Code generation naming            | 9.0/10 | 9.5/10 | Updated to reference new namespace paths           |
| Code generation architecture      | 9.5/10 | 9.5/10 | Unchanged — already strong                         |
| Developer comfort                 | 9.0/10 | 10/10  | `areg::String`, `areg::Component`, `areg::ResultType` — everything one `areg::` away |
| Container API surface             | 8.0/10 | 8.5/10 | Evaluation deferred; Concurrent naming is cleaner  |
| **Overall**                       | **9.3/10** | **9.8/10** | Simpler namespace (2 vs 7) improves developer comfort |

**What prevents 10.0/10:**
- Container wrappers around STL types still exist (evaluation deferred — section 10)
- Whether to keep or remove them is an API design decision, not a naming decision

**What makes this design match top C++ frameworks:**
- `areg::String`, `areg::Thread`, `areg::Component`, `areg::ResultType` — everything one namespace deep, like `absl::Status`, `std::thread`
- `areg::HashMap<K,V>`, `areg::ConcurrentStack<T>` — Java/C# developers recognize these immediately
- `areg::os::MutexPosix`, `areg::os::SpinLockWin32` — platform suffix is self-documenting
- Zero project-specific prefixes (no IE, TE, NE, s, e, E_) — pure C++ community standard naming
- Flat namespace reflects the architecture: threading, IPC, and logging are one unified system, not separate subsystems
- The naming is invisible — developers think about their domain, not about decoding Areg conventions
