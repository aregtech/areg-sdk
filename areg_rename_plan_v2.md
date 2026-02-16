# Areg SDK — Rename Plan v2

**Goal:** Score 90+/100 against modern C++ community standards.
**Principles applied:**

1. **Interfaces get the clean name** when users implement them (they're the contract).
   Concrete classes get a qualifier only if they collide.
2. **No suffix conventions** — no `Base`, `If`, `Data`, `Kind`. Every name stands on its own meaning.
3. **Namespaces are semantic** — match the domain, not the directory. Allow underscores per coding rules.
4. **Resolve collisions by renaming the less-important side** — usually the concrete/internal class, not the interface.
5. **Fix all typos** in the same pass.

---

## Design Decisions

### Interface collision strategy

When `IEFoo` collides with concrete `Foo`:

| Pattern | Example | Reasoning |
|---------|---------|-----------|
| Interface = clean noun | `ByteBuffer` (was `ByteBuffer`) | Users implement/extend this — it's the contract |
| Concrete = qualified noun | `SharedByteBuffer` (was concrete using `ByteBuffer`) | Concrete describes HOW, interface describes WHAT |
| Struct = physical/raw qualifier | `RawByteBuffer` (was `sByteBuffer`) | Struct is the raw memory layout |

### Namespace strategy

- 12 namespaces (down from 19, up from 9)
- Merge tightly-coupled pairs (`NELogging` + `NELogOptions` -> `logging`)
- Keep distinct domains separate (`math`, `memory`, `socket` stay split from `base`)
- Use full words, allow underscores per coding rules
- All under `areg::` when `AREG_NAMESPACE=1`

---

## 1. IE-Prefixed Classes (Interfaces)

Drop `IE`. Give interface the natural name. Resolve collisions on the concrete side.

| Old Name                          | New Name                        | Old File Path                                                    | New File Path                                                    | Collision Resolution                       |
|-----------------------------------|---------------------------------|------------------------------------------------------------------|------------------------------------------------------------------|--------------------------------------------|
| `ByteBuffer`                    | `ByteBuffer`                    | `framework/areg/base/ByteBuffer.hpp`                           | `framework/areg/base/ByteBuffer.hpp`                             | Struct `sByteBuffer` -> `RawByteBuffer`    |
| `ConfigListener`         | `ConfigListener`                | `framework/areg/persist/ConfigListener.hpp`             | `framework/areg/persist/ConfigListener.hpp`                      | No collision                               |
| `Cursor`                | `Cursor`                        | `framework/areg/base/Cursor.hpp`                       | `framework/areg/base/Cursor.hpp`                                 | Short, clear (std::streambuf uses same)    |
| `DatabaseEngine`                | `DatabaseEngine`                | `framework/areg/persist/DatabaseEngine.hpp`                    | `framework/areg/persist/DatabaseEngine.hpp`                      | No collision                               |
| `EventConsumer`                 | `EventConsumer`                 | `framework/areg/component/EventConsumer.hpp`                   | `framework/areg/component/EventConsumer.hpp`                     | No collision                               |
| `EventRouter`                   | `EventRouter`                   | `framework/areg/component/EventRouter.hpp`                     | `framework/areg/component/EventRouter.hpp`                       | No collision                               |
| `ObjectBase`                 | `GenericObject`                 | `framework/areg/base/ObjectBase.hpp`                        | `framework/areg/base/GenericObject.hpp`                          | No collision                               |
| `IOStream`                      | `IOStream`                      | `framework/areg/base/IOStream.hpp`                             | `framework/areg/base/IOStream.hpp`                               | No collision                               |
| `InStream`                      | `InStream`                      | `framework/areg/base/IOStream.hpp`                             | (same file as IOStream)                                          | No collision                               |
| `LogLayout`                        | `LogLayout`                     | `framework/areg/logging/private/Layouts.hpp`                     | (same file)                                                      | Scoped to logging context                  |
| `LogDatabaseEngine`             | `LogDatabaseEngine`             | `framework/areg/logging/LogDatabaseEngine.hpp`                 | `framework/areg/logging/LogDatabaseEngine.hpp`                   | No collision                               |
| `NotificationConsumer`     | `NotificationConsumer`          | `framework/areg/component/NotificationEvent.hpp`                 | (same file)                                                      | No collision                               |
| `OutStream`                     | `OutStream`                     | `framework/areg/base/IOStream.hpp`                             | (same file as IOStream)                                          | No collision                               |
| `ProxyEventConsumer`            | `ProxyEventConsumer`            | `framework/areg/component/ProxyEvent.hpp`                        | (same file)                                                      | No collision                               |
| `ProxyListener`                 | `ProxyListener`                 | `framework/areg/component/ProxyListener.hpp`                   | `framework/areg/component/ProxyListener.hpp`                     | No collision                               |
| `QueueListener`                 | `QueueListener`                 | `framework/areg/component/private/QueueListener.hpp`           | `framework/areg/component/private/QueueListener.hpp`             | No collision                               |
| `RemoteEventConsumer`           | `RemoteEventConsumer`           | `framework/areg/component/RemoteEventConsumer.hpp`             | `framework/areg/component/RemoteEventConsumer.hpp`               | No collision                               |
| `RemoteMessageHandler`          | `RemoteMessageHandler`          | `framework/areg/ipc/RemoteMessageHandler.hpp`                  | `framework/areg/ipc/RemoteMessageHandler.hpp`                    | No collision                               |
| `Lockable`                  | `Lockable`                      | `framework/areg/base/SyncObjects.hpp`                            | (same file)                                                      | C++ concept name; concrete = `ResourceLock`|
| `ConnectionConsumer`     | `ServiceConnectionConsumer`     | `framework/areg/ipc/ConnectionConsumer.hpp`             | `framework/areg/ipc/ServiceConnectionConsumer.hpp`               | No collision                               |
| `ConnectionHandler`      | `ServiceConnectionHandler`      | `framework/aregextend/service/ConnectionHandler.hpp`    | `framework/aregextend/service/ServiceConnectionHandler.hpp`      | No collision                               |
| `ConnectionProvider`     | `ServiceConnectionProvider`     | `framework/areg/ipc/ConnectionProvider.hpp`             | `framework/areg/ipc/ServiceConnectionProvider.hpp`               | No collision                               |
| `ServiceEventConsumer`      | `ServiceEventConsumer`          | `framework/areg/ipc/ServiceEventConsumer.hpp`                | `framework/areg/ipc/ServiceEventConsumer.hpp`                    | Drop redundant "Base"                      |
| `RegistrationConsumer`       | `ServiceRegisterConsumer`       | `framework/areg/ipc/RegistrationConsumer.hpp`               | `framework/areg/ipc/ServiceRegisterConsumer.hpp`                 | No collision                               |
| `RegistrationProvider`       | `ServiceRegisterProvider`       | `framework/areg/ipc/RegistrationProvider.hpp`               | `framework/areg/ipc/ServiceRegisterProvider.hpp`                 | No collision                               |
| `StubEventConsumer`             | `StubEventConsumer`             | `framework/areg/component/StubEvent.hpp`                         | (same file)                                                      | No collision                               |
| `SyncObject`                    | `SyncObject`                    | `framework/areg/base/SyncObject.hpp`                           | `framework/areg/base/SyncObject.hpp`                             | Enum `eSyncObject` -> `SyncCategory`       |
| `SyncObjectPosix`              | `SyncObjectBaseIX`              | `framework/areg/base/private/posix/SyncObjectPosix.hpp`       | `framework/areg/base/private/posix/SyncObjectBaseIX.hpp`         | Platform-internal, IX suffix stays         |
| `ThreadConsumer`                | `ThreadConsumer`                | `framework/areg/base/ThreadConsumer.hpp`                       | `framework/areg/base/ThreadConsumer.hpp`                         | No collision                               |
| `TimerConsumer`                 | `TimerConsumer`                 | `framework/areg/component/TimerConsumer.hpp`                   | `framework/areg/component/TimerConsumer.hpp`                     | No collision                               |
| `WaitablePosix`                | `WaitableBaseIX`                | `framework/areg/base/private/posix/WaitablePosix.hpp`         | `framework/areg/base/private/posix/WaitableBaseIX.hpp`           | Platform-internal, IX suffix stays         |
| `WorkerThreadConsumer`          | `WorkerThreadConsumer`          | `framework/areg/component/WorkerThreadConsumer.hpp`            | `framework/areg/component/WorkerThreadConsumer.hpp`              | No collision                               |

---

## 2. TE-Prefixed Classes (Templates)

Drop `TE`. Resolve collisions with meaningful qualifiers (not `Base`).

| Old Name                          | New Name                        | Old File Path                                                    | New File Path                                                    | Notes                                      |
|-----------------------------------|---------------------------------|------------------------------------------------------------------|------------------------------------------------------------------|--------------------------------------------|
| `ArrayList`                     | `ArrayList`                     | `framework/areg/base/ArrayList.hpp`                            | `framework/areg/base/ArrayList.hpp`                              |                                            |
| `FixedArray`                    | `FixedArray`                    | `framework/areg/base/FixedArray.hpp`                           | `framework/areg/base/FixedArray.hpp`                             |                                            |
| `HashMap`                       | `HashMap`                       | `framework/areg/base/HashMap.hpp`                              | `framework/areg/base/HashMap.hpp`                                |                                            |
| `IdHashMap`                     | `IdHashMap`                     | `framework/areg/base/Containers.hpp`                             | (same file)                                                      |                                            |
| `IdMap`                         | `IdMap`                         | `framework/areg/base/Containers.hpp`                             | (same file)                                                      |                                            |
| `IntegerHashMap`                | `IntHashMap`                    | `framework/areg/base/Containers.hpp`                             | (same file)                                                      |                                            |
| `IntegerMap`                    | `IntMap`                        | `framework/areg/base/Containers.hpp`                             | (same file)                                                      |                                            |
| `LinkedList`                    | `LinkedList`                    | `framework/areg/base/LinkedList.hpp`                           | `framework/areg/base/LinkedList.hpp`                             |                                            |
| `ConcurrentResourceListMap`           | `LockResourceListMap`           | `framework/areg/base/ResourceListMap.hpp`                      | (same file)                                                      |                                            |
| `ConcurrentResourceMap`               | `LockResourceMap`               | `framework/areg/base/ResourceMap.hpp`                          | (same file)                                                      |                                            |
| `ConcurrentRingStack`                 | `LockRingStack`                 | `framework/areg/base/RingStack.hpp`                            | (same file)                                                      |                                            |
| `ConcurrentRuntimeResourceMap`        | `LockRuntimeResourceMap`        | `framework/areg/base/RuntimeResourceMap.hpp`                   | (same file)                                                      |                                            |
| `ConcurrentStack`                     | `LockStack`                     | `framework/areg/base/Stack.hpp`                                | (same file)                                                      |                                            |
| `OrderedMap`                           | `OrderedMap`                    | `framework/areg/base/OrderedMap.hpp`                                  | `framework/areg/base/OrderedMap.hpp`                             | Wraps `std::map` (ordered). More idiomatic |
| `ResourceListMap`         | `NolockResourceListMap`         | `framework/areg/base/ResourceListMap.hpp`                      | (same file)                                                      |                                            |
| `ResourceMap`             | `NolockResourceMap`             | `framework/areg/base/ResourceMap.hpp`                          | (same file)                                                      |                                            |
| `RingStack`               | `NolockRingStack`               | `framework/areg/base/RingStack.hpp`                            | (same file)                                                      |                                            |
| `RuntimeResourceMap`      | `NolockRuntimeResourceMap`      | `framework/areg/base/RuntimeResourceMap.hpp`                   | (same file)                                                      |                                            |
| `Stack`                   | `NolockStack`                   | `framework/areg/base/Stack.hpp`                                | (same file)                                                      |                                            |
| `PtrHashMap`                | `PtrHashMap`                    | `framework/areg/base/Containers.hpp`                             | (same file)                                                      |                                            |
| `PtrMap`                    | `PtrMap`                        | `framework/areg/base/Containers.hpp`                             | (same file)                                                      |                                            |
| `KeyValuePair`                      | `KeyValuePair`                  | `framework/areg/base/KeyValuePair.hpp`                             | `framework/areg/base/KeyValuePair.hpp`                           | Generic template, clear purpose            |
| `ResourceListMapBase`               | `ResourceListMap`               | `framework/areg/base/ResourceListMap.hpp`                      | `framework/areg/base/ResourceListMap.hpp`                        |                                            |
| `ResourceListMapImpl`           | `ResourceListMapImpl`           | `framework/areg/base/TemplateBase.hpp`                         | (same file)                                                      |                                            |
| `ResourceMapBase`                   | `ResourceMap`                   | `framework/areg/base/ResourceMap.hpp`                          | `framework/areg/base/ResourceMap.hpp`                            |                                            |
| `ResourceMapImpl`               | `ResourceMapImpl`               | `framework/areg/base/TemplateBase.hpp`                         | (same file)                                                      |                                            |
| `RingStack`                     | `RingStack`                     | `framework/areg/base/RingStack.hpp`                            | `framework/areg/base/RingStack.hpp`                              |                                            |
| `RuntimeHashMap`                | `RuntimeHashMap`                | `framework/areg/base/RuntimeResourceMap.hpp`                   | (same file)                                                      |                                            |
| `RuntimeResourceMapBase`            | `RuntimeResourceMap`            | `framework/areg/base/RuntimeResourceMap.hpp`                   | `framework/areg/base/RuntimeResourceMap.hpp`                     |                                            |
| `SortedLinkedList`              | `SortedLinkedList`              | `framework/areg/base/SortedLinkedList.hpp`                     | `framework/areg/base/SortedLinkedList.hpp`                       |                                            |
| `StackBase`                         | `Stack`                         | `framework/areg/base/Stack.hpp`                                | `framework/areg/base/Stack.hpp`                                  |                                            |
| `StringBase`                        | `StringTemplate`                | `framework/areg/base/StringBase.hpp`                               | `framework/areg/base/StringTemplate.hpp`                         | `String`/`WideString` inherit from this    |
| `StringHashMap`                 | `StringHashMap`                 | `framework/areg/base/Containers.hpp`                             | (same file)                                                      |                                            |
| `StringMap`                     | `StringMap`                     | `framework/areg/base/Containers.hpp`                             | (same file)                                                      |                                            |
| `Align`                         | `Align`                         | `framework/areg/base/NEMemory.hpp`                               | (same file, struct in namespace)                                 |                                            |

---

## 3. Structs

Drop prefixes. Use domain-specific qualifiers for collisions — no `Data` suffix.

| Old Name (inner / alias)                         | New Name                   | File                                                             | Notes                                      |
|--------------------------------------------------|----------------------------|------------------------------------------------------------------|--------------------------------------------|
| `BufferHeader` / `BufferHeader`             | `BufferHeader`             | `framework/areg/base/NEMemory.hpp`                               | Fix typo Bufer->Buffer                     |
| `S_ByteBuffer` / `sByteBuffer`                   | `RawByteBuffer`            | `framework/areg/base/NEMemory.hpp`                               | `ByteBuffer` taken by interface rename     |
| `S_RemoteMessage` / `sRemoteMessage`             | `RemoteMessageFrame`       | `framework/areg/base/NEMemory.hpp`                               | Wire format = "frame"                      |
| `S_RemoteMessageHeader` / `sRemoteMessageHeader` | `RemoteMessageHeader`      | `framework/areg/base/NEMemory.hpp`                               | No collision                               |
| `S_Rect` / `sRect`                               | `Rect`                     | `framework/areg/base/NEMath.hpp`                                 |                                            |
| `S_Size` / `sSize`                               | `Size`                     | `framework/areg/base/NEMath.hpp`                                 |                                            |
| `S_Point` / `sPoint`                             | `Point`                    | `framework/areg/base/NEMath.hpp`                                 |                                            |
| `S_Coord` / `sCoord`                             | `Coord`                    | `framework/areg/base/NEMath.hpp`                                 |                                            |
| `S_SystemTime` / `sSystemTime`                   | `SystemTime`               | `framework/areg/base/NEUtilities.hpp`                            |                                            |
| `S_InterfaceData` / `SInterfaceData`             | `InterfaceDescriptor`      | `framework/areg/component/NEService.hpp`                         | Describes a service interface               |
| `sLargeInteger`                                  | `LargeInteger`             | `framework/areg/base/NEMath.hpp`                                 |                                            |
| `sEntryTypesEnabling`                            | `EntryTypesEnabling`       | `framework/areg/appbase/AppDefs.hpp`                       |                                            |
| `sLogInstance`                                   | `LogInstance`              | `framework/areglogger/client/LogObserverApi.h`                   |                                            |
| `sLogMessage`                                    | `LogRecord`                | `framework/areg/logging/NELogging.hpp`                           | Wire format; `LogMessage` is the class     |
| `sLogScope`                                      | `LogScopeEntry`            | `framework/areglogger/client/LogObserverApi.h`                   | `LogScope` is the class; entry = one item  |
| `sObserverEvents`                                | `ObserverEvents`           | `framework/areglogger/client/LogObserverApi.h`                   |                                            |
| `sObserverStatus`                                | `ObserverStatus`           | `framework/logobserver/app/LogObserver.hpp`                      |                                            |
| `sOption`                                        | `Option`                   | `framework/aregextend/console/OptionParser.hpp`                  |                                            |
| `sOptionSetup`                                   | `OptionSetup`              | `framework/aregextend/console/OptionParser.hpp`                  |                                            |
| `sProperty`                                      | `PropertyRecord`           | `framework/areg/persist/NEPersistence.hpp`                       | Serialized form; `Property` is the class   |
| `sPropertyKey`                                   | `PropertyKeyRecord`        | `framework/areg/persist/NEPersistence.hpp`                       | Same reasoning                             |
| `sScopeFilter`                                   | `ScopeFilter`              | `framework/aregextend/db/LogSqliteDatabase.hpp`                  |                                            |
| `sScopeInfo`                                     | `ScopeInfo`                | `framework/areg/logging/NELogging.hpp`                           |                                            |
| `sServiceConnectedInstance`                      | `ServiceConnectedInstance` | `framework/areg/component/NEService.hpp`                         |                                            |
| `BufferAllocator`                                | (keep)                     | `framework/areg/base/NEMemory.hpp`                               | Already clean                              |
| `BufferDeleter`                                  | (keep)                     | `framework/areg/base/NEMemory.hpp`                               | Already clean                              |
| `Range`                                          | (keep)                     | `framework/aregextend/console/OptionParser.hpp`                  | Already clean                              |

---

## 4. Enums

Drop `e`/`E_` prefixes. PascalCase. Use `Mode`/`Type`/`Status` for qualifiers (not `Kind`).
Merge typedef inner/alias pairs into one name.

| Old Name (inner / alias)                         | New Name                   | File                                                             | Notes                                      |
|--------------------------------------------------|----------------------------|------------------------------------------------------------------|--------------------------------------------|
| `E_AppState` / `eApplicationState`               | `AppState`                 | `framework/areg/appbase/AppDefs.hpp`                       |                                            |
| `E_CharDefs` / `eCharDefs`                       | `CharDefs`                 | `framework/areg/base/NEString.hpp`                               |                                            |
| `E_CompareResult` / `eCompare`                   | `CompareResult`            | `framework/areg/base/NEMath.hpp`                                 |                                            |
| `E_CompletionStatus` / `eCompletionStatus`       | `CompletionStatus`         | `framework/areg/base/Thread.hpp`                                 |                                            |
| `E_ConfigKey` / `eConfigKey`                     | `ConfigKey`                | `framework/areg/logging/private/NELogOptions.hpp`                |                                            |
| `E_ConnectionBehavior` / `eConnectionBehavior`   | `ConnectionBehavior`       | `framework/aregextend/service/ServiceCommunicationBase.hpp`       |                                            |
| `E_Cookies` / `eCookies`                         | `CookieId`                 | `framework/areg/base/NECommon.hpp`                               | Descriptive                                |
| `E_CursorPosition` / `eCursorPosition`           | `SeekOrigin`               | `framework/areg/base/Cursor.hpp`                       | Matches POSIX/C# convention                |
| `E_DataStateType` / `eDataStateType`             | `DataState`                | `framework/areg/component/NEService.hpp`                         |                                            |
| `E_DegubPrio` / `DebugPriority`                     | `DebugPriority`            | `framework/areg/base/private/NEDebug.hpp`                        | Fix typo                                   |
| `E_DigitSign` / `eDigitSign`                     | `DigitSign`                | `framework/areg/base/NEMath.hpp`                                 |                                            |
| `E_DisconnectReason` / `eDisconnectReason`       | `DisconnectReason`         | `framework/areg/component/NEService.hpp`                         |                                            |
| `E_EventData` / `eEventData`                     | `EventPayload`             | `framework/areg/component/EventDataStream.hpp`                   | `EventData` is a class; payload = content  |
| `E_EventFired` / `eEventFired`                   | `EventFired`               | `framework/areg/base/private/posix/SyncLockAndWaitPosix.hpp`        |                                            |
| `E_EventOrder` / `eEventOrder`                   | `EventOrder`               | `framework/areg/component/private/EventDispatcherBase.hpp`       |                                            |
| `E_EventPriority` / `eEventPriority`             | `EventPriority`            | `framework/areg/component/Event.hpp`                             |                                            |
| `E_EventResetInfo` / `eEventResetInfo`           | `EventResetInfo`           | `framework/areg/base/private/posix/NESyncTypesIX.hpp`            |                                            |
| `E_EventType` / `eEventType`                     | `EventType`                | `framework/areg/component/Event.hpp`                             |                                            |
| `E_ExitCodes` / `eExitCodes`                     | `ExitCode`                 | `framework/areg/base/ThreadConsumer.hpp`                       | Singular                                   |
| `E_FileOpenBits` / `eFileOpenBits`               | `FileOpenBits`             | `framework/areg/base/FileBase.hpp`                               |                                            |
| `E_FileOpenMode` / `eFileOpenMode`               | `FileOpenMode`             | `framework/areg/base/FileBase.hpp`                               |                                            |
| `E_FuncIdRange` / `eFuncIdRange`                 | `FuncIdRange`              | `framework/areg/component/NEService.hpp`                         |                                            |
| `E_InstanceBitness` / `eInstanceBitness`         | `InstanceBitness`          | `framework/areg/component/NEService.hpp`                         |                                            |
| `E_Layouts` / `eLayouts`                         | `LayoutTag`                | `framework/areg/logging/private/NELogOptions.hpp`                | Identifies layout tokens in format strings |
| `E_LogStatus` / `eLogStatus`                     | `LogStatus`                | `framework/areg/logging/private/NELogOptions.hpp`                |                                            |
| `E_LoggingAction` / `eLoggingAction`             | `LoggingAction`            | `framework/areg/logging/private/LoggingEvent.hpp`                |                                            |
| `E_MatchCondition` / `eMatchCondition`           | `MatchCondition`           | `framework/areg/base/private/posix/NESyncTypesIX.hpp`            |                                            |
| `E_MessageDataType` / `eMessageDataType`         | `MessageDataType`          | `framework/areg/component/NEService.hpp`                         |                                            |
| `E_MessageSource` / `eMessageSource`             | `MessageSource`            | `framework/areg/component/NEService.hpp`                         |                                            |
| `E_ProcEnv` / `eProcEnv`                         | `ProcessEnv`               | `framework/areg/base/Process.hpp`                                |                                            |
| `E_Radix` / `eRadix`                             | `Radix`                    | `framework/areg/base/NEString.hpp`                               |                                            |
| `E_RequestType` / `eRequestType`                 | `RequestType`              | `framework/areg/component/NEService.hpp`                         |                                            |
| `E_ServiceCall` / `eServiceCalls`                | `ServiceCall`              | `framework/areg/component/NEService.hpp`                         |                                            |
| `E_ServiceConnection` / `eServiceConnection`     | `ServiceConnection`        | `framework/areg/component/NEService.hpp`                         |                                            |
| `E_ServiceControl` / `eServiceControl`           | `ServiceControl`           | `framework/aregextend/service/SystemServiceBase.hpp`             |                                            |
| `E_ServiceManagerCommands` / `eServiceManagerCommands` | `ServiceManagerCommand` | `framework/areg/component/private/ServiceManagerEvents.hpp` | Singular                                   |
| `E_ServiceRequestType` / `eServiceRequestType`   | `ServiceRequestType`       | `framework/areg/component/NEService.hpp`                         |                                            |
| `E_ServiceType` / `eServiceType`                 | `ServiceType`              | `framework/areg/component/NEService.hpp`                         |                                            |
| `E_SpecialFolder` / `eSpecialFolder`             | `SpecialFolder`            | `framework/areg/base/File.hpp`                                   |                                            |
| `E_SyncObject` / `eSyncObject`                   | `SyncCategory`             | `framework/areg/base/SyncObject.hpp`                           | `SyncObject` taken by class                |
| `E_SyncObjectFired` / `eSyncObjectFired`         | `SyncObjectFired`          | `framework/areg/base/private/posix/NESyncTypesIX.hpp`            |                                            |
| `E_ThreadPriority` / `eThreadPriority`           | `ThreadPriority`           | `framework/areg/base/Thread.hpp`                                 |                                            |
| `E_WaitType` / `eWaitType`                       | `WaitType`                 | `framework/areg/base/private/posix/SyncLockAndWaitPosix.hpp`        |                                            |
| `ResultType`                                     | (keep)                     | `framework/areg/component/NEService.hpp`                         | Already clean                              |
| `eBufferType`                                    | `BufferType`               | `framework/areg/base/NEMemory.hpp`                               |                                            |
| `eColumnType`                                    | `ColumnType`               | `framework/aregextend/db/SqliteStatement.hpp`                    |                                            |
| `eConfigKeys`                                    | `ConfigKeys`               | `framework/areg/persist/NEPersistence.hpp`                       |                                            |
| `eConnectionState`                               | `ConnectionState`          | `framework/areg/ipc/ServiceClientConnectionBase.hpp`             |                                            |
| `eConnectionTypes`                               | `ConnectionType`           | `framework/areg/ipc/NERemoteService.hpp`                         | Singular                                   |
| `Grouping`                                     | `Grouping`                 | `framework/areg/logging/private/ScopeNodeBase.hpp`               | Fix typo                                   |
| `eIndex`                                         | `IndexPosition`            | `framework/areg/base/NECommon.hpp`                               |                                            |
| `eLockedState`                                   | `LockedState`              | `framework/areg/base/SyncObjects.hpp`                            |                                            |
| `eLogDataType`                                   | `LogDataType`              | `framework/areg/logging/NELogging.hpp`                           |                                            |
| `eLogMessageType`                                | `LogMessageType`           | `framework/areg/logging/NELogging.hpp`                           |                                            |
| `eLogPriority`                                   | `LogPriority`              | `framework/areg/logging/NELogging.hpp`                           |                                            |
| `eLogType`                                       | `LogType`                  | `framework/areglogger/client/LogObserverApi.h`                   |                                            |
| `eLoggerOptions`                                 | `LoggerOption`             | `framework/logcollector/app/LogCollector.hpp`                    | Singular                                   |
| `LoggingType`                                   | `LoggingType`              | `framework/areg/logging/NELogging.hpp`                           | Fix typo, singular                         |
| `eMessageResult`                                 | `MessageResult`            | `framework/areg/base/NEMemory.hpp`                               |                                            |
| `eModelState`                                    | `ModelState`               | `framework/areg/component/NERegistry.hpp`                        |                                            |
| `eNode`                                          | `NodeType`                 | `framework/areg/logging/private/ScopeNodeBase.hpp`               |                                            |
| `eObserverStates`                                | `ObserverState`            | `framework/areglogger/client/LogObserverApi.h`                   | Singular                                   |
| `eQueryResult`                                   | `QueryResult`              | `framework/aregextend/db/SqliteStatement.hpp`                    |                                            |
| `eRemoteConnection`                              | `RemoteConnection`         | `framework/areg/ipc/NERemoteService.hpp`                         |                                            |
| `eRemoteServices`                                | `RemoteServiceId`          | `framework/areg/ipc/NERemoteService.hpp`                         | Identifies which remote service            |
| `eRingOverlap`                                   | `RingOverlap`              | `framework/areg/base/NECommon.hpp`                               |                                            |
| `eRouterOptions`                                 | `RouterOption`             | `framework/mtrouter/app/MultitargetRouter.hpp`                   | Singular                                   |
| `eSendMessage`                                   | `SendPolicy`               | `framework/areg/ipc/SendMessageEvent.hpp`                        | Describes HOW to send                      |
| `eServiceEventCommands`                          | `ServiceEventCommand`      | `framework/areg/ipc/ServiceEvent.hpp`                            | Singular                                   |
| `eServiceOption`                                 | `ServiceOption`            | `framework/aregextend/service/NESystemService.hpp`               |                                            |
| `eSort`                                          | `SortOrder`                | `framework/areg/base/NECommon.hpp`                               |                                            |
| `eSystemServiceState`                            | `SystemServiceState`       | `framework/aregextend/service/NESystemService.hpp`               |                                            |
| `eTimerType`                                     | `TimerType`                | `framework/areg/component/TimerBase.hpp`                         |                                            |
| `eValidFlags`                                    | `ValidFlags`               | `framework/aregextend/console/OptionParser.hpp`                  |                                            |
| `eWaitResult`                                    | `WaitResult`               | `framework/areg/base/SyncObjects.hpp`                            |                                            |

---

## 5. Namespaces

Semantic grouping. Full words. Underscores allowed per coding rules.
All live under `areg::` when compiled with `AREG_NAMESPACE=1`.

| Old Name                          | New Name           | Primary File                                                     | What it contains                                    |
|-----------------------------------|--------------------|------------------------------------------------------------------|-----------------------------------------------------|
| `NEApplication`                   | `application`      | `framework/areg/appbase/AppDefs.hpp`                       | App lifecycle, entry types                          |
| `NECommon`                        | `common`           | `framework/areg/base/NECommon.hpp`                               | Shared types/constants (SortOrder, CookieId, etc.)  |
| `NEMath`                          | `math`             | `framework/areg/base/NEMath.hpp`                                 | Rect, Point, Size, Coord, LargeInteger              |
| `NEMemory`                        | `memory`           | `framework/areg/base/NEMemory.hpp`                               | Buffer types, alignment, message frames             |
| `NESocket`                        | `socket`           | `framework/areg/base/NESocket.hpp`                               | SocketAddress, UserData                             |
| `NEString`                        | `strings`          | `framework/areg/base/NEString.hpp`                               | CharDefs, Radix, string utilities                   |
| `NEUtilities`                     | `util`             | `framework/areg/base/NEUtilities.hpp`                            | SystemTime, Duration, misc helpers                  |
| `NEDebug`                         | `debug`            | `framework/areg/base/private/NEDebug.hpp`                        | Debug priorities, exception handling                |
| `NESyncTypesIX`                   | `sync_posix`       | `framework/areg/base/private/posix/NESyncTypesIX.hpp`            | POSIX sync primitives (platform-internal)           |
| `NERegistry`                      | `registry`         | `framework/areg/component/NERegistry.hpp`                        | Model, ComponentEntry, ServiceEntry, etc.           |
| `NEService`                       | `service`          | `framework/areg/component/NEService.hpp`                         | Service types, enums, ProxyData, StateArray          |
| `NEConnection`                    | `connection`       | `framework/areg/ipc/private/NEConnection.hpp`                    | Connection constants                                |
| `NERemoteService`                 | `remote_service`   | `framework/areg/ipc/NERemoteService.hpp`                         | Remote connection types and IDs                     |
| `NELogging`                       | `logging`          | `framework/areg/logging/NELogging.hpp`                           | Log types, priorities, ScopeInfo, LogRecord          |
| `NELogOptions`                    | `log_options`      | `framework/areg/logging/private/NELogOptions.hpp`                | Log config keys, layout tokens, status              |
| `NEPersistence`                   | `persist`          | `framework/areg/persist/NEPersistence.hpp`                       | PropertyRecord, ConfigKeys                          |
| `NESystemService`                 | `system_service`   | `framework/aregextend/service/NESystemService.hpp`               | Service options, system service states               |
| `NELogCollectorSettings`          | `log_collector`    | `framework/logcollector/app/NELogCollectorSettings.hpp`          | Log collector configuration                         |
| `NEMultitargetRouterSettings`     | `router_config`    | `framework/mtrouter/app/NEMultitargetRouterSettings.hpp`         | Router configuration                                |

**Result:** `areg::memory::RawByteBuffer`, `areg::logging::LogPriority`, `areg::service::RequestType`, `areg::math::Rect`.

---

## 6. Typos Fixed

| Old (typo)                  | Fixed                          | File                                                             |
|-----------------------------|--------------------------------|------------------------------------------------------------------|
| `BufferHeader`              | `BufferHeader`                 | `framework/areg/base/NEMemory.hpp`                               |
| `BufferHeader`         | `BufferHeader`                 | `framework/areg/base/NEMemory.hpp`                               |
| `DayTimeLayout`            | `DayTimeLayout`                | `framework/areg/logging/private/Layouts.hpp`                     |
| `Grouping`                | `Grouping`                     | `framework/areg/logging/private/ScopeNodeBase.hpp`               |
| `LoggingType`              | `LoggingType`                  | `framework/areg/logging/NELogging.hpp`                           |
| `DebugPriority`                | `DebugPriority`                | `framework/areg/base/private/NEDebug.hpp`                        |
| `ServiceCommunicationBase`   | `ServiceCommunicationBase`     | `framework/aregextend/service/ServiceCommunicationBase.hpp`       |

---

## 7. Summary

| Category     | Renamed | Kept as-is | Collisions resolved by               |
|--------------|---------|------------|---------------------------------------|
| IE classes   | 32      | 0          | Rename concrete/struct, not interface |
| TE classes   | 35      | 0          | Meaningful qualifiers                 |
| Structs      | 25      | 3          | Domain words: Frame, Record, Raw      |
| Enums        | ~90     | 1          | Mode, Policy, Origin, Tag, Payload    |
| Namespaces   | 19      | 0          | Full words, underscores allowed       |
| Typos        | 7       | —          | —                                     |
| **Total**    | **~208**|            |                                       |

---

## 8. How It Reads (Before/After Examples)

```cpp
// BEFORE
NEMemory::sByteBuffer buf;
Lockable& lock = getLock();
HashMap<String, NEService::eServiceType> map;
if (state == NELogging::eLogPriority::LogPrioDebug) { ... }

// AFTER (with AREG_NAMESPACE=1)
areg::memory::RawByteBuffer buf;
Lockable& lock = get_lock();
HashMap<String, service::ServiceType> map;
if (state == logging::LogPriority::Debug) { ... }

// AFTER (without AREG_NAMESPACE)
memory::RawByteBuffer buf;
Lockable& lock = get_lock();
HashMap<String, service::ServiceType> map;
if (state == logging::LogPriority::Debug) { ... }
```
