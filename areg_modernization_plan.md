# Areg SDK — Modernization Plan

**Version:** 2.0
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
| 3 | **PascalCase for types, snake_case for namespaces.** Per `docs/AREG_AI_CODING_RULES.md`. | `BufferHeader`, `areg::component` |
| 4 | **Always use `areg::` namespace.** Remove `AREG_NAMESPACE=0` option. Framework types always live in `areg::`. | `areg::component::ResultType` — always |
| 5 | **Generated code belongs to the user's namespace.** Generated service classes reference framework types with `areg::` qualification. No re-exports. | `class HelloProvider : public areg::StubBase` |
| 6 | **Fix all typos in the same pass.** No separate "typo fix" phase. | `sBuferHeader` -> `BufferHeader` |
| 7 | **Flat fundamentals, broad subsystems.** Base types live directly in `areg::`. Only distinct subsystems get sub-namespaces. Maximum 2 namespace levels. | `areg::String`, `areg::Thread`, `areg::component::ResultType` |
| 8 | **Namespace provides context — names don't repeat it.** When a type lives in a namespace, drop the namespace's concept from the type name. | `areg::ConnectionProvider` not `areg::ServiceConnectionProvider` |
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
using areg::component::ResultType;

// Namespace alias
namespace comp = areg::component;
comp::ResultType result = comp::ResultType::Success;

// Using-directive in .cpp files only (never in headers)
using namespace areg;
```

### 2.2 Namespace hierarchy (7 namespaces)

Fundamental types from `framework/areg/base/` and `framework/areg/appbase/` live directly in `areg::`.
Only distinct subsystems get their own sub-namespace. This matches Boost/Asio/gRPC convention.

```
areg::                       // Fundamentals: String, Thread, Mutex, ByteBuffer, Rect,
                             //   Point, Size, SystemTime, HashMap, ArrayList, Stack,
                             //   AppState, SortOrder, CompareResult, Radix, etc.
                             //   (everything from base/ and appbase/)
areg::component::            // Component/service system: ResultType, ServiceType,
                             //   RequestType, ModelState, ComponentEntry, etc.
                             //   (everything from component/)
areg::net::                  // Networking/IPC: RemoteConnection, ConnectionType,
                             //   ConnectionState, RemoteServiceId, etc.
                             //   (everything from ipc/)
areg::log::                  // Logging: LogPriority, LogRecord, ScopeInfo, ConfigKey,
                             //   LayoutTag, LogStatus, Grouping, NodeType, etc.
                             //   (everything from logging/)
areg::persist::              // Persistence: ConfigKeys, PropertyRecord, etc.
                             //   (everything from persist/)
areg::ext::                  // Extended library: ServiceOption, SystemServiceState,
                             //   ConnectionBehavior, ServiceControl, etc.
                             //   (everything from aregextend/)
areg::os::                   // OS abstraction layer: platform-specific primitives
                             //   MutexPosix, SpinLockWin32, SyncObjectPosix, etc.
                             //   Class names carry platform suffix; only one compiles per platform

mtrouter::                   // Router application (not in areg::)
logcollector::               // Log collector application (not in areg::)
```

**Why this is better than 17 sub-namespaces:**
- A developer typing `areg::` sees fundamentals immediately — no need to guess `areg::strings::` vs `areg::common::` vs `areg::util::`
- Subsystems (`component`, `net`, `log`, `persist`) map directly to directory structure
- Maximum 2 levels deep: `areg::component::ResultType` — never `areg::component::service::ResultType`
- Only 7 namespaces for users to learn (`os::` is internal but visible in platform code)

### 2.3 Previous namespace → new location mapping

| Old NE Namespace | Types move to | Rationale |
|---|---|---|
| NEApplication | `areg::` | Small set, fundamental (AppState, EntryTypesEnabling) |
| NECommon | `areg::` | Constants, SortOrder, IndexPosition, CookieId, RingOverlap |
| NEMath | `areg::` | Rect, Point, Size, Coord, LargeInteger, CompareResult, DigitSign |
| NEMemory | `areg::` | BufferHeader, RawByteBuffer, BufferType, MessageResult, Align |
| NESocket | `areg::` | SocketAddress, UserData |
| NEString | `areg::` | CharDefs, Radix |
| NEUtilities | `areg::` | SystemTime |
| NEDebug | `areg::` | DebugPriority (simple enum, private by file location) |
| NESyncTypesIX | `areg::os::` | POSIX sync types (platform suffix on class names) |
| NERegistry | `areg::component::` | ModelState, ComponentEntry, ServiceEntry |
| NEService | `areg::component::` | ResultType, ServiceType, RequestType, DataState, etc. |
| NEConnection | `areg::net::` (internal, private by file location) | Connection constants |
| NERemoteService | `areg::net::` | RemoteConnection, ConnectionType, RemoteServiceId |
| NELogging | `areg::log::` | LogPriority, LogRecord, ScopeInfo, LogDataType, etc. |
| NELogOptions | `areg::log::` (internal, private by file location) | ConfigKey, LayoutTag, LogStatus |
| NEPersistence | `areg::persist::` | ConfigKeys, PropertyRecord |
| NESystemService | `areg::ext::` | ServiceOption, SystemServiceState |
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
| `Lockable`                  | `Lockable`                      | `areg/base/SyncObjects.hpp`                                      | (same file)                                                      | Concrete `ResourceLock` keeps its name     |
| `ConnectionConsumer`     | `ConnectionConsumer`            | `areg/ipc/ConnectionConsumer.hpp`                       | `areg/ipc/ConnectionConsumer.hpp`                                | In `areg::`, "Service" is redundant        |
| `ConnectionHandler`      | `ConnectionHandler`             | `aregextend/service/ConnectionHandler.hpp`              | `aregextend/service/ConnectionHandler.hpp`                       | In `areg::ext::`, "Service" is redundant   |
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
| `TEArrayList`                     | `ArrayList`                     | `areg/base/TEArrayList.hpp`                                      | `areg/base/ArrayList.hpp`                                        |                                            |
| `TEFixedArray`                    | `FixedArray`                    | `areg/base/TEFixedArray.hpp`                                     | `areg/base/FixedArray.hpp`                                       |                                            |
| `TEHashMap`                       | `HashMap`                       | `areg/base/TEHashMap.hpp`                                        | `areg/base/HashMap.hpp`                                          |                                            |
| `TEIdHashMap`                     | `IdHashMap`                     | `areg/base/Containers.hpp`                                       | (same file)                                                      |                                            |
| `TEIdMap`                         | `IdMap`                         | `areg/base/Containers.hpp`                                       | (same file)                                                      |                                            |
| `TEIntegerHashMap`                | `IntHashMap`                    | `areg/base/Containers.hpp`                                       | (same file)                                                      |                                            |
| `TEIntegerMap`                    | `IntMap`                        | `areg/base/Containers.hpp`                                       | (same file)                                                      |                                            |
| `TELinkedList`                    | `LinkedList`                    | `areg/base/TELinkedList.hpp`                                     | `areg/base/LinkedList.hpp`                                       |                                            |
| `TELockResourceListMap`           | `ConcurrentResourceListMap`     | `areg/base/TEResourceListMap.hpp`                                | (same file)                                                      | `Lock` -> `Concurrent`                     |
| `TELockResourceMap`               | `ConcurrentResourceMap`         | `areg/base/TEResourceMap.hpp`                                    | (same file)                                                      | `Lock` -> `Concurrent`                     |
| `TELockRingStack`                 | `ConcurrentRingStack`           | `areg/base/TERingStack.hpp`                                      | (same file)                                                      | `Lock` -> `Concurrent`                     |
| `TELockRuntimeResourceMap`        | `ConcurrentRuntimeResourceMap`  | `areg/base/TERuntimeResourceMap.hpp`                             | (same file)                                                      | `Lock` -> `Concurrent`                     |
| `TELockStack`                     | `ConcurrentStack`               | `areg/base/TEStack.hpp`                                          | (same file)                                                      | `Lock` -> `Concurrent`                     |
| `TEMap`                           | `OrderedMap`                    | `areg/base/TEMap.hpp`                                            | `areg/base/OrderedMap.hpp`                                       | Wraps `std::map` (ordered)                 |
| `TENolockResourceListMap`         | `ResourceListMap`               | `areg/base/TEResourceListMap.hpp`                                | (same file)                                                      | `Nolock` dropped — unsync is the default   |
| `TENolockResourceMap`             | `ResourceMap`                   | `areg/base/TEResourceMap.hpp`                                    | (same file)                                                      | `Nolock` dropped                           |
| `TENolockRingStack`               | `RingStack`                     | `areg/base/TERingStack.hpp`                                      | (same file)                                                      | `Nolock` dropped (same as base template)   |
| `TENolockRuntimeResourceMap`      | `RuntimeResourceMap`            | `areg/base/TERuntimeResourceMap.hpp`                             | (same file)                                                      | `Nolock` dropped                           |
| `TENolockStack`                   | `Stack`                         | `areg/base/TEStack.hpp`                                          | (same file)                                                      | `Nolock` dropped (same as base template)   |
| `TEObjectFactory`                 | `ObjectFactory`                 | `areg/base/ObjectBase.hpp`                                  | (moves with ObjectBase)                                          |                                            |
| `TEPointerHashMap`                | `PtrHashMap`                    | `areg/base/Containers.hpp`                                       | (same file)                                                      |                                            |
| `TEPointerMap`                    | `PtrMap`                        | `areg/base/Containers.hpp`                                       | (same file)                                                      |                                            |
| `TEProperty`                      | `KeyValuePair`                  | `areg/base/TEProperty.hpp`                                       | `areg/base/KeyValuePair.hpp`                                     |                                            |
| `TEResourceListMap`               | `ResourceListMapBase`           | `areg/base/TEResourceListMap.hpp`                                | `areg/base/ResourceListMap.hpp`                                  | Base template; aliases are `ResourceListMap` and `ConcurrentResourceListMap` |
| `TEResourceListMapImpl`           | `ResourceListMapImpl`           | `areg/base/TETemplateBase.hpp`                                   | (same file)                                                      |                                            |
| `TEResourceMap`                   | `ResourceMapBase`               | `areg/base/TEResourceMap.hpp`                                    | `areg/base/ResourceMap.hpp`                                      | Base template; aliases are `ResourceMap` and `ConcurrentResourceMap` |
| `TEResourceMapImpl`               | `ResourceMapImpl`               | `areg/base/TETemplateBase.hpp`                                   | (same file)                                                      |                                            |
| `TERingStack`                     | `RingStackBase`                 | `areg/base/TERingStack.hpp`                                      | `areg/base/RingStack.hpp`                                        | Base template; aliases are `RingStack` and `ConcurrentRingStack` |
| `TERuntimeHashMap`                | `RuntimeHashMap`                | `areg/base/TERuntimeResourceMap.hpp`                             | (same file)                                                      |                                            |
| `TERuntimeResourceMap`            | `RuntimeResourceMapBase`        | `areg/base/TERuntimeResourceMap.hpp`                             | `areg/base/RuntimeResourceMap.hpp`                               | Base template; aliases are `RuntimeResourceMap` and `ConcurrentRuntimeResourceMap` |
| `TESortedLinkedList`              | `SortedLinkedList`              | `areg/base/TESortedLinkedList.hpp`                               | `areg/base/SortedLinkedList.hpp`                                 |                                            |
| `TEStack`                         | `StackBase`                     | `areg/base/TEStack.hpp`                                          | `areg/base/Stack.hpp`                                            | Base template; aliases are `Stack` and `ConcurrentStack` |
| `TEString`                        | `StringTemplate`                | `areg/base/TEString.hpp`                                         | `areg/base/StringTemplate.hpp`                                   | `String`/`WideString` inherit from this    |
| `TEStringHashMap`                 | `StringHashMap`                 | `areg/base/Containers.hpp`                                       | (same file)                                                      |                                            |
| `TEStringMap`                     | `StringMap`                     | `areg/base/Containers.hpp`                                       | (same file)                                                      |                                            |
| `TEAlign`                         | `Align`                         | `areg/base/NEMemory.hpp`                                         | (stays in `areg::` namespace)                                    |                                            |

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
| `S_BuferHeaderInfo` / `sBuferHeader`             | `BufferHeader`             | `areg/base/NEMemory.hpp`                          | Fix typo Bufer -> Buffer                   |
| `S_ByteBuffer` / `sByteBuffer`                   | `RawByteBuffer`            | `areg/base/NEMemory.hpp`                          | `ByteBuffer` is the interface class        |
| `S_RemoteMessage` / `sRemoteMessage`             | `MessageFrame`             | `areg/base/NEMemory.hpp`                          | Wire format = "frame"; context already implies remote |
| `S_RemoteMessageHeader` / `sRemoteMessageHeader` | `MessageHeader`            | `areg/base/NEMemory.hpp`                          | Context already implies remote             |
| `S_Rect` / `sRect`                               | `Rect`                     | `areg/base/NEMath.hpp`                            |                                            |
| `S_Size` / `sSize`                               | `Size`                     | `areg/base/NEMath.hpp`                            |                                            |
| `S_Point` / `sPoint`                             | `Point`                    | `areg/base/NEMath.hpp`                            |                                            |
| `S_Coord` / `sCoord`                             | `Coord`                    | `areg/base/NEMath.hpp`                            |                                            |
| `S_SystemTime` / `sSystemTime`                   | `SystemTime`               | `areg/base/NEUtilities.hpp`                       |                                            |
| `S_InterfaceData` / `SInterfaceData`             | `InterfaceDescriptor`      | `areg/component/NEService.hpp`                    | Describes a service interface               |
| `sLargeInteger`                                  | `LargeInteger`             | `areg/base/NEMath.hpp`                            |                                            |
| `sEntryTypesEnabling`                            | `EntryTypesEnabling`       | `areg/appbase/NEApplication.hpp`                  |                                            |
| `sLogInstance`                                   | `LogInstance`              | `areglogger/client/LogObserverApi.h`              |                                            |
| `sLogMessage`                                    | `LogRecord`                | `areg/logging/NELogging.hpp`                      | Wire format; `LogMessage` is the class     |
| `sLogScope`                                      | `LogScopeEntry`            | `areglogger/client/LogObserverApi.h`              | `LogScope` is the class                    |
| `sObserverEvents`                                | `ObserverEvents`           | `areglogger/client/LogObserverApi.h`              |                                            |
| `sObserverStatus`                                | `ObserverStatus`           | `logobserver/app/LogObserver.hpp`                 |                                            |
| `sOption`                                        | `Option`                   | `aregextend/console/OptionParser.hpp`             |                                            |
| `sOptionSetup`                                   | `OptionSetup`              | `aregextend/console/OptionParser.hpp`             |                                            |
| `sProperty`                                      | `PropertyRecord`           | `areg/persist/NEPersistence.hpp`                  | Serialized form; `Property` is the class   |
| `sPropertyKey`                                   | `PropertyKeyRecord`        | `areg/persist/NEPersistence.hpp`                  | Same reasoning                             |
| `sScopeFilter`                                   | `ScopeFilter`              | `aregextend/db/LogSqliteDatabase.hpp`             |                                            |
| `sScopeInfo`                                     | `ScopeInfo`                | `areg/logging/NELogging.hpp`                      |                                            |
| `sServiceConnectedInstance`                      | `ConnectedInstance`        | `areg/component/NEService.hpp`                    | In `areg::component::`, "Service" is redundant |
| `BufferAllocator`                                | (keep)                     | `areg/base/NEMemory.hpp`                          |                                            |
| `BufferDeleter`                                  | (keep)                     | `areg/base/NEMemory.hpp`                          |                                            |
| `Range`                                          | (keep)                     | `aregextend/console/OptionParser.hpp`             |                                            |

**Total: 24 renames, 3 kept**

**Changes from v1.0:** `RemoteMessageFrame` -> `MessageFrame`, `RemoteMessageHeader` -> `MessageHeader`, `ServiceConnectedInstance` -> `ConnectedInstance` (namespace provides context).

---

## 6. Enums

Drop `e`/`E_` prefixes. PascalCase. Merge typedef inner/alias pairs.
Redundant "Service" prefix dropped from names in `areg::component::` namespace.

| Old Name (inner / alias)                         | New Name                   | Namespace          | File                                              | Notes                                      |
|--------------------------------------------------|----------------------------|--------------------|---------------------------------------------------|--------------------------------------------|
| `E_AppState` / `eApplicationState`               | `AppState`                 | `areg::`           | `areg/appbase/NEApplication.hpp`                  |                                            |
| `E_CharDefs` / `eCharDefs`                       | `CharDefs`                 | `areg::`           | `areg/base/NEString.hpp`                          |                                            |
| `E_CompareResult` / `eCompare`                   | `CompareResult`            | `areg::`           | `areg/base/NEMath.hpp`                            |                                            |
| `E_CompletionStatus` / `eCompletionStatus`       | `CompletionStatus`         | `areg::`           | `areg/base/Thread.hpp`                            |                                            |
| `E_ConfigKey` / `eConfigKey`                     | `ConfigKey`                | `areg::log::`      | `areg/logging/private/NELogOptions.hpp`           |                                            |
| `E_ConnectionBehavior` / `eConnectionBehavior`   | `ConnectionBehavior`       | `areg::ext::`      | `aregextend/service/ServiceCommunicatonBase.hpp`  |                                            |
| `E_Cookies` / `eCookies`                         | `CookieId`                 | `areg::`           | `areg/base/NECommon.hpp`                          |                                            |
| `E_CursorPosition` / `eCursorPosition`           | `SeekOrigin`               | `areg::`           | `areg/base/Cursor.hpp`                  | Matches POSIX/C# convention                |
| `E_DataStateType` / `eDataStateType`             | `DataState`                | `areg::component::`| `areg/component/NEService.hpp`                    |                                            |
| `E_DegubPrio` / `eDegubPrio`                     | `DebugPriority`            | `areg::`   | `areg/base/private/NEDebug.hpp`                   | Fix typo                                   |
| `E_DigitSign` / `eDigitSign`                     | `DigitSign`                | `areg::`           | `areg/base/NEMath.hpp`                            |                                            |
| `E_DisconnectReason` / `eDisconnectReason`       | `DisconnectReason`         | `areg::component::`| `areg/component/NEService.hpp`                    |                                            |
| `E_EventData` / `eEventData`                     | `EventPayload`             | `areg::component::`| `areg/component/EventDataStream.hpp`              | `EventData` is a class                     |
| `E_EventFired` / `eEventFired`                   | `EventFired`               | `areg::os::` | `areg/base/private/posix/SyncLockAndWaitPosix.hpp` |                                          |
| `E_EventOrder` / `eEventOrder`                   | `EventOrder`               | `areg::component::`| `areg/component/private/EventDispatcherBase.hpp`  |                                            |
| `E_EventPriority` / `eEventPriority`             | `EventPriority`            | `areg::component::`| `areg/component/Event.hpp`                        |                                            |
| `E_EventResetInfo` / `eEventResetInfo`           | `EventResetInfo`           | `areg::os::` | `areg/base/private/posix/NESyncTypesIX.hpp` |                                            |
| `E_EventType` / `eEventType`                     | `EventType`                | `areg::component::`| `areg/component/Event.hpp`                        |                                            |
| `E_ExitCodes` / `eExitCodes`                     | `ExitCode`                 | `areg::`           | `areg/base/ThreadConsumer.hpp`                  | Singular                                   |
| `E_FileOpenBits` / `eFileOpenBits`               | `FileOpenBits`             | `areg::`           | `areg/base/FileBase.hpp`                          |                                            |
| `E_FileOpenMode` / `eFileOpenMode`               | `FileOpenMode`             | `areg::`           | `areg/base/FileBase.hpp`                          |                                            |
| `E_FuncIdRange` / `eFuncIdRange`                 | `FuncIdRange`              | `areg::component::`| `areg/component/NEService.hpp`                    |                                            |
| `E_InstanceBitness` / `eInstanceBitness`         | `InstanceBitness`          | `areg::component::`| `areg/component/NEService.hpp`                    |                                            |
| `E_Layouts` / `eLayouts`                         | `LayoutTag`                | `areg::log::`      | `areg/logging/private/NELogOptions.hpp`           |                                            |
| `E_LogStatus` / `eLogStatus`                     | `LogStatus`                | `areg::log::`      | `areg/logging/private/NELogOptions.hpp`           |                                            |
| `E_LoggingAction` / `eLoggingAction`             | `LoggingAction`            | `areg::log::`      | `areg/logging/private/LoggingEvent.hpp`           |                                            |
| `E_MatchCondition` / `eMatchCondition`           | `MatchCondition`           | `areg::os::` | `areg/base/private/posix/NESyncTypesIX.hpp` |                                            |
| `E_MessageDataType` / `eMessageDataType`         | `MessageDataType`          | `areg::component::`| `areg/component/NEService.hpp`                    |                                            |
| `E_MessageSource` / `eMessageSource`             | `MessageSource`            | `areg::component::`| `areg/component/NEService.hpp`                    |                                            |
| `E_ProcEnv` / `eProcEnv`                         | `ProcessEnv`               | `areg::`           | `areg/base/Process.hpp`                           |                                            |
| `E_Radix` / `eRadix`                             | `Radix`                    | `areg::`           | `areg/base/NEString.hpp`                          |                                            |
| `E_RequestType` / `eRequestType`                 | `RequestType`              | `areg::component::`| `areg/component/NEService.hpp`                    |                                            |
| `E_ServiceCall` / `eServiceCalls`                | `ServiceCall`              | `areg::component::`| `areg/component/NEService.hpp`                    | Singular                                   |
| `E_ServiceConnection` / `eServiceConnection`     | `ConnectionStatus`         | `areg::component::`| `areg/component/NEService.hpp`                    | "Service" dropped; "Status" more precise than "Connection" as enum |
| `E_ServiceControl` / `eServiceControl`           | `ServiceControl`           | `areg::ext::`      | `aregextend/service/SystemServiceBase.hpp`        |                                            |
| `E_ServiceManagerCommands` / `eServiceManagerCommands` | `ManagerCommand`    | `areg::component::`| `areg/component/private/ServiceManagerEvents.hpp` | Singular; in component::, "Service" redundant |
| `E_ServiceRequestType` / `eServiceRequestType`   | `ServiceRequestType`       | `areg::component::`| `areg/component/NEService.hpp`                    |                                            |
| `E_ServiceType` / `eServiceType`                 | `ServiceType`              | `areg::component::`| `areg/component/NEService.hpp`                    |                                            |
| `E_SpecialFolder` / `eSpecialFolder`             | `SpecialFolder`            | `areg::`           | `areg/base/File.hpp`                              |                                            |
| `E_SyncObject` / `eSyncObject`                   | `SyncCategory`             | `areg::`           | `areg/base/SyncObject.hpp`                      | `SyncObject` taken by class                |
| `E_SyncObjectFired` / `eSyncObjectFired`         | `SyncObjectFired`          | `areg::os::` | `areg/base/private/posix/NESyncTypesIX.hpp` |                                            |
| `E_ThreadPriority` / `eThreadPriority`           | `ThreadPriority`           | `areg::`           | `areg/base/Thread.hpp`                            |                                            |
| `E_WaitType` / `eWaitType`                       | `WaitType`                 | `areg::os::` | `areg/base/private/posix/SyncLockAndWaitPosix.hpp` |                                          |
| `ResultType`                                     | (keep)                     | `areg::component::`| `areg/component/NEService.hpp`                    |                                            |
| `eBufferType`                                    | `BufferType`               | `areg::`           | `areg/base/NEMemory.hpp`                          |                                            |
| `eColumnType`                                    | `ColumnType`               | `areg::ext::`      | `aregextend/db/SqliteStatement.hpp`               |                                            |
| `eConfigKeys`                                    | `ConfigKeys`               | `areg::persist::`  | `areg/persist/NEPersistence.hpp`                  |                                            |
| `eConnectionState`                               | `ConnectionState`          | `areg::net::`      | `areg/ipc/ServiceClientConnectionBase.hpp`        |                                            |
| `eConnectionTypes`                               | `ConnectionType`           | `areg::net::`      | `areg/ipc/NERemoteService.hpp`                    | Singular                                   |
| `eGroupping`                                     | `Grouping`                 | `areg::log::`      | `areg/logging/private/ScopeNodeBase.hpp`          | Fix typo                                   |
| `eIndex`                                         | `IndexPosition`            | `areg::`           | `areg/base/NECommon.hpp`                          |                                            |
| `eLockedState`                                   | `LockedState`              | `areg::`           | `areg/base/SyncObjects.hpp`                       |                                            |
| `eLogDataType`                                   | `LogDataType`              | `areg::log::`      | `areg/logging/NELogging.hpp`                      |                                            |
| `eLogMessageType`                                | `LogMessageType`           | `areg::log::`      | `areg/logging/NELogging.hpp`                      |                                            |
| `eLogPriority`                                   | `LogPriority`              | `areg::log::`      | `areg/logging/NELogging.hpp`                      |                                            |
| `eLogType`                                       | `LogType`                  | `areg::log::`      | `areglogger/client/LogObserverApi.h`              |                                            |
| `eLoggerOptions`                                 | `LoggerOption`             | `logcollector::`   | `logcollector/app/LogCollector.hpp`               | Singular                                   |
| `eLogingTypes`                                   | `LoggingType`              | `areg::log::`      | `areg/logging/NELogging.hpp`                      | Fix typo, singular                         |
| `eMessageResult`                                 | `MessageResult`            | `areg::`           | `areg/base/NEMemory.hpp`                          |                                            |
| `eModelState`                                    | `ModelState`               | `areg::component::`| `areg/component/NERegistry.hpp`                   |                                            |
| `eNode`                                          | `NodeType`                 | `areg::log::`      | `areg/logging/private/ScopeNodeBase.hpp`          |                                            |
| `eObserverStates`                                | `ObserverState`            | `areg::log::`      | `areglogger/client/LogObserverApi.h`              | Singular                                   |
| `eQueryResult`                                   | `QueryResult`              | `areg::ext::`      | `aregextend/db/SqliteStatement.hpp`               |                                            |
| `eRemoteConnection`                              | `RemoteConnection`         | `areg::net::`      | `areg/ipc/NERemoteService.hpp`                    |                                            |
| `eRemoteServices`                                | `RemoteServiceId`          | `areg::net::`      | `areg/ipc/NERemoteService.hpp`                    |                                            |
| `eRingOverlap`                                   | `RingOverlap`              | `areg::`           | `areg/base/NECommon.hpp`                          |                                            |
| `eRouterOptions`                                 | `RouterOption`             | `mtrouter::`       | `mtrouter/app/MultitargetRouter.hpp`              | Singular                                   |
| `eSendMessage`                                   | `SendPolicy`               | `areg::net::`      | `areg/ipc/SendMessageEvent.hpp`                   |                                            |
| `eServiceEventCommands`                          | `EventCommand`             | `areg::net::`      | `areg/ipc/ServiceEvent.hpp`                       | Singular; "Service" dropped (in net::)     |
| `eServiceOption`                                 | `ServiceOption`            | `areg::ext::`      | `aregextend/service/NESystemService.hpp`          |                                            |
| `eSort`                                          | `SortOrder`                | `areg::`           | `areg/base/NECommon.hpp`                          |                                            |
| `eSystemServiceState`                            | `SystemServiceState`       | `areg::ext::`      | `aregextend/service/NESystemService.hpp`          |                                            |
| `eTimerType`                                     | `TimerType`                | `areg::component::`| `areg/component/TimerBase.hpp`                    |                                            |
| `eValidFlags`                                    | `ValidFlags`               | `areg::ext::`      | `aregextend/console/OptionParser.hpp`             |                                            |
| `eWaitResult`                                    | `WaitResult`               | `areg::`           | `areg/base/SyncObjects.hpp`                       |                                            |

**Total: ~90 renames (deduplicated pairs), 1 kept**

**Changes from v1.0:** Added namespace column for clarity. `ServiceConnection` -> `ConnectionStatus` (more precise). `ServiceManagerCommand` -> `ManagerCommand`. `ServiceEventCommand` -> `EventCommand`. All POSIX internals explicitly in `areg::os::`.

---

## 7. Namespaces

File renames — the NE prefix is dropped. The C++ namespace identifier changes per section 2.3.

| Old NE Namespace | New Namespace | Old Primary File | New Primary File |
|---|---|---|---|
| `NEApplication` | `areg::` (flat) | `areg/appbase/NEApplication.hpp` | `areg/appbase/Application.hpp` |
| `NECommon` | `areg::` (flat) | `areg/base/NECommon.hpp` | `areg/base/Common.hpp` |
| `NEMath` | `areg::` (flat) | `areg/base/NEMath.hpp` | `areg/base/Math.hpp` |
| `NEMemory` | `areg::` (flat) | `areg/base/NEMemory.hpp` | `areg/base/Memory.hpp` |
| `NESocket` | `areg::` (flat) | `areg/base/NESocket.hpp` | `areg/base/Socket.hpp` |
| `NEString` | `areg::` (flat) | `areg/base/NEString.hpp` | `areg/base/Strings.hpp` |
| `NEUtilities` | `areg::` (flat) | `areg/base/NEUtilities.hpp` | `areg/base/Utilities.hpp` |
| `NEDebug` | `areg::` (flat) | `areg/base/private/NEDebug.hpp` | `areg/base/private/Debug.hpp` |
| `NESyncTypesIX` | `areg::os::` | `areg/base/private/posix/NESyncTypesIX.hpp` | `areg/base/private/posix/SyncTypesPosix.hpp` |
| `NERegistry` | `areg::component::` | `areg/component/NERegistry.hpp` | `areg/component/Registry.hpp` |
| `NEService` | `areg::component::` | `areg/component/NEService.hpp` | `areg/component/Service.hpp` |
| `NEConnection` | `areg::net::` (internal) | `areg/ipc/private/NEConnection.hpp` | `areg/ipc/private/Connection.hpp` |
| `NERemoteService` | `areg::net::` | `areg/ipc/NERemoteService.hpp` | `areg/ipc/RemoteService.hpp` |
| `NELogging` | `areg::log::` | `areg/logging/NELogging.hpp` | `areg/logging/Logging.hpp` |
| `NELogOptions` | `areg::log::` (internal) | `areg/logging/private/NELogOptions.hpp` | `areg/logging/private/LogOptions.hpp` |
| `NEPersistence` | `areg::persist::` | `areg/persist/NEPersistence.hpp` | `areg/persist/Persistence.hpp` |
| `NESystemService` | `areg::ext::` | `aregextend/service/NESystemService.hpp` | `aregextend/service/SystemService.hpp` |
| `NELogCollectorSettings` | `logcollector::` | `logcollector/app/NELogCollectorSettings.hpp` | `logcollector/app/LogCollectorSettings.hpp` |
| `NEMultitargetRouterSettings` | `mtrouter::` | `mtrouter/app/NEMultitargetRouterSettings.hpp` | `mtrouter/app/MultitargetRouterSettings.hpp` |

**Total: 19 renames**

**Changes from v1.0:** File names use PascalCase (matching type naming convention) instead of snake_case. 17 separate sub-namespaces collapsed into 7 namespaces (6 public + `os::`). Types from `base/` and `appbase/` merge into flat `areg::`. `areg::detail::` eliminated — replaced by `areg::os::` for platform code, other internals use semantic namespaces with `private/` directory for access control.

---

## 8. Typos Fixed

These are corrected as part of the renames above. Listed here for tracking.

| Old (typo)                  | Corrected                      | File                                              |
|-----------------------------|--------------------------------|---------------------------------------------------|
| `sBuferHeader`              | `BufferHeader`                 | `areg/base/NEMemory.hpp`                          |
| `S_BuferHeaderInfo`         | `BufferHeader`                 | `areg/base/NEMemory.hpp`                          |
| `DayTimeLaytout`            | `DayTimeLayout`                | `areg/logging/private/Layouts.hpp`                |
| `eGroupping`                | `Grouping`                     | `areg/logging/private/ScopeNodeBase.hpp`          |
| `eLogingTypes`              | `LoggingType`                  | `areg/logging/NELogging.hpp`                      |
| `eDegubPrio`                | `DebugPriority`                | `areg/base/private/NEDebug.hpp`                   |
| `ServiceCommunicatonBase`   | `ServiceCommunicationBase`     | `aregextend/service/ServiceCommunicatonBase.hpp`  |

**Total: 7 typos**

---

## 9. Code Generation

### 9.1 Always `areg::` namespace

Generated code references framework types with full `areg::` qualification. With the new namespace structure, `ResultType` is in `areg::component::`:

```cpp
class HelloServiceProvider : public areg::StubBase
{
    virtual void response_hello(areg::component::ResultType result) = 0;
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
        response_hello(areg::component::ResultType::Success);
    }
};
```

```cpp
// Developer implements consumer
#include "generated/HelloServiceConsumer.hpp"

class HelloClient : public HelloServiceConsumer
{
    void on_hello_response(areg::component::ResultType result) override
    {
        if (result == areg::component::ResultType::Success)
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
- [ ] Collapse 17 NE sub-namespaces into 6 public namespaces (per section 2.2)
- [ ] Move base/ types into flat `areg::` (no sub-namespace for NECommon, NEMath, NEMemory, NEString, NEUtilities, NESocket, NEApplication)
- [ ] Move component/ types into `areg::component::`
- [ ] Move ipc/ types into `areg::net::`
- [ ] Move logging/ types into `areg::log::`
- [ ] Move persist/ types into `areg::persist::`
- [ ] Move aregextend/ types into `areg::ext::`
- [ ] Move internal types into `areg::` and `areg::os::`
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
- [ ] Update code generator to use `areg::component::ResultType` (new namespace path)
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
| Namespace naming                  | 9.0/10 | 10/10  | 6 broad namespaces instead of 17; matches Boost/Asio |
| Namespace architecture            | 9.5/10 | 10/10  | Flat fundamentals, broad subsystems, os:: for platform code |
| Code generation naming            | 9.0/10 | 9.5/10 | Updated to reference new namespace paths           |
| Code generation architecture      | 9.5/10 | 9.5/10 | Unchanged — already strong                         |
| Developer comfort                 | 9.0/10 | 10/10  | `areg::String`, `areg::Thread` — zero learning curve |
| Container API surface             | 8.0/10 | 8.5/10 | Evaluation deferred; Concurrent naming is cleaner  |
| **Overall**                       | **9.3/10** | **9.7/10** | |

**What prevents 10.0/10:**
- Container wrappers around STL types still exist (evaluation deferred — section 10)
- Whether to keep or remove them is an API design decision, not a naming decision

**What makes this design match top C++ frameworks:**
- `areg::String`, `areg::Thread`, `areg::Mutex` — identical depth to `boost::asio::io_context`, `std::thread`
- `areg::component::ResultType` — identical depth to `boost::beast::http::status`
- `areg::HashMap<K,V>`, `areg::ConcurrentStack<T>` — Java/C# developers recognize these immediately
- `areg::os::MutexPosix`, `areg::os::SpinLockWin32` — platform suffix is self-documenting
- Zero project-specific prefixes (no IE, TE, NE, s, e, E_) — pure C++ community standard naming
- The naming is invisible — developers think about their domain, not about decoding Areg conventions
