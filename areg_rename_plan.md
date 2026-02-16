# Areg SDK — Rename Plan

Generated for the style migration: remove legacy prefixes (`IE`, `TE`, `NE`, `E_`, `e`, `s`, `S_`),
adopt PascalCase for types, snake_case for namespaces per `docs/AREG_AI_CODING_RULES.md`.

---

## 1. IE-Prefixed Classes (Interfaces)

These are abstract base classes / interfaces. Drop `IE`, keep meaningful name.
Where dropping `IE` would collide with an existing concrete class, add a suffix or rephrase.

| Old Name                          | New Name                        | Old File                                                         | New File                                                         | Notes                                      |
|-----------------------------------|---------------------------------|------------------------------------------------------------------|------------------------------------------------------------------|--------------------------------------------|
| `ByteBuffer`                    | `ByteBufferBase`                | `framework/areg/base/ByteBuffer.hpp`                           | `framework/areg/base/ByteBufferBase.hpp`                         | `ByteBuffer` taken by struct alias         |
| `ConfigListener`         | `ConfigListener`                | `framework/areg/persist/ConfigListener.hpp`             | `framework/areg/persist/ConfigListener.hpp`                      | Shorter, clear                             |
| `Cursor`                | `CursorPosition`                | `framework/areg/base/Cursor.hpp`                       | `framework/areg/base/CursorPosition.hpp`                         |                                            |
| `DatabaseEngine`                | `DatabaseEngine`                | `framework/areg/persist/DatabaseEngine.hpp`                    | `framework/areg/persist/DatabaseEngine.hpp`                      |                                            |
| `EventConsumer`                 | `EventConsumer`                 | `framework/areg/component/EventConsumer.hpp`                   | `framework/areg/component/EventConsumer.hpp`                     |                                            |
| `EventRouter`                   | `EventRouter`                   | `framework/areg/component/EventRouter.hpp`                     | `framework/areg/component/EventRouter.hpp`                       |                                            |
| `IOStream`                      | `IOStream`                      | `framework/areg/base/IOStream.hpp`                             | `framework/areg/base/IOStream.hpp`                               |                                            |
| `InStream`                      | `InStream`                      | `framework/areg/base/IOStream.hpp`                             | (same file as IOStream)                                          |                                            |
| `LogLayout`                        | `LogLayout`                     | `framework/areg/logging/private/Layouts.hpp`                     | (same file)                                                      | `Layout` too generic; context = logging    |
| `LogDatabaseEngine`             | `LogDatabaseEngine`             | `framework/areg/logging/LogDatabaseEngine.hpp`                 | `framework/areg/logging/LogDatabaseEngine.hpp`                   |                                            |
| `NotificationConsumer`     | `NotificationConsumer`          | `framework/areg/component/NotificationEvent.hpp`                 | (same file)                                                      | Shorter                                    |
| `OutStream`                     | `OutStream`                     | `framework/areg/base/IOStream.hpp`                             | (same file as IOStream)                                          |                                            |
| `ProxyEventConsumer`            | `ProxyEventConsumer`            | `framework/areg/component/ProxyEvent.hpp`                        | (same file)                                                      |                                            |
| `ProxyListener`                 | `ProxyListener`                 | `framework/areg/component/ProxyListener.hpp`                   | `framework/areg/component/ProxyListener.hpp`                     |                                            |
| `QueueListener`                 | `QueueListener`                 | `framework/areg/component/private/QueueListener.hpp`           | `framework/areg/component/private/QueueListener.hpp`             |                                            |
| `RemoteEventConsumer`           | `RemoteEventConsumer`           | `framework/areg/component/RemoteEventConsumer.hpp`             | `framework/areg/component/RemoteEventConsumer.hpp`               |                                            |
| `RemoteMessageHandler`          | `RemoteMessageHandler`          | `framework/areg/ipc/RemoteMessageHandler.hpp`                  | `framework/areg/ipc/RemoteMessageHandler.hpp`                    |                                            |
| `Lockable`                  | `ResourceLockBase`              | `framework/areg/base/SyncPrimitives.hpp`                            | (same file)                                                      | `ResourceLock` exists as concrete class    |
| `ConnectionConsumer`     | `ServiceConnectionConsumer`     | `framework/areg/ipc/ConnectionConsumer.hpp`             | `framework/areg/ipc/ServiceConnectionConsumer.hpp`               |                                            |
| `ConnectionHandler`      | `ServiceConnectionHandler`      | `framework/aregextend/service/ConnectionHandler.hpp`    | `framework/aregextend/service/ServiceConnectionHandler.hpp`      |                                            |
| `ConnectionProvider`     | `ServiceConnectionProvider`     | `framework/areg/ipc/ConnectionProvider.hpp`             | `framework/areg/ipc/ServiceConnectionProvider.hpp`               |                                            |
| `ServiceEventConsumer`      | `ServiceEventConsumer`      | `framework/areg/ipc/ServiceEventConsumer.hpp`                | (same file, class rename only)                                   |                                            |
| `RegistrationConsumer`       | `ServiceRegisterConsumer`       | `framework/areg/ipc/RegistrationConsumer.hpp`               | `framework/areg/ipc/ServiceRegisterConsumer.hpp`                 |                                            |
| `RegistrationProvider`       | `ServiceRegisterProvider`       | `framework/areg/ipc/RegistrationProvider.hpp`               | `framework/areg/ipc/ServiceRegisterProvider.hpp`                 |                                            |
| `StubEventConsumer`             | `StubEventConsumer`             | `framework/areg/component/StubEvent.hpp`                         | (same file)                                                      |                                            |
| `SyncObject`                    | `SyncObject`                    | `framework/areg/base/SyncObject.hpp`                           | `framework/areg/base/SyncObject.hpp`                             |                                            |
| `SyncObjectPosix`              | `SyncObjectBaseIX`              | `framework/areg/base/private/posix/SyncObjectPosix.hpp`       | `framework/areg/base/private/posix/SyncObjectBaseIX.hpp`         |                                            |
| `ThreadConsumer`                | `ThreadConsumer`                | `framework/areg/base/ThreadConsumer.hpp`                       | `framework/areg/base/ThreadConsumer.hpp`                         |                                            |
| `TimerConsumer`                 | `TimerConsumer`                 | `framework/areg/component/TimerConsumer.hpp`                   | `framework/areg/component/TimerConsumer.hpp`                     |                                            |
| `WaitablePosix`                | `WaitableBaseIX`                | `framework/areg/base/private/posix/WaitablePosix.hpp`         | `framework/areg/base/private/posix/WaitableBaseIX.hpp`           |                                            |
| `WorkerThreadConsumer`          | `WorkerThreadConsumer`          | `framework/areg/component/WorkerThreadConsumer.hpp`            | `framework/areg/component/WorkerThreadConsumer.hpp`              |                                            |

---

## 2. TE-Prefixed Classes (Templates)

These are template classes. Drop `TE` prefix. Where collisions exist, note them.

| Old Name                          | New Name                        | Old File                                                         | New File                                                         | Notes                                      |
|-----------------------------------|---------------------------------|------------------------------------------------------------------|------------------------------------------------------------------|--------------------------------------------|
| `ArrayList`                     | `ArrayList`                     | `framework/areg/base/ArrayList.hpp`                            | `framework/areg/base/ArrayList.hpp`                              |                                            |
| `FixedArray`                    | `FixedArray`                    | `framework/areg/base/FixedArray.hpp`                           | `framework/areg/base/FixedArray.hpp`                             |                                            |
| `HashMap`                       | `HashMap`                       | `framework/areg/base/HashMap.hpp`                              | `framework/areg/base/HashMap.hpp`                                |                                            |
| `IdHashMap`                     | `IdHashMap`                     | `framework/areg/base/Containers.hpp`                             | (same file)                                                      |                                            |
| `IdMap`                         | `IdMap`                         | `framework/areg/base/Containers.hpp`                             | (same file)                                                      |                                            |
| `IntegerHashMap`                | `IntHashMap`                    | `framework/areg/base/Containers.hpp`                             | (same file)                                                      | Shorter                                    |
| `IntegerMap`                    | `IntMap`                        | `framework/areg/base/Containers.hpp`                             | (same file)                                                      | Shorter                                    |
| `LinkedList`                    | `LinkedList`                    | `framework/areg/base/LinkedList.hpp`                           | `framework/areg/base/LinkedList.hpp`                             |                                            |
| `ConcurrentResourceListMap`           | `LockResourceListMap`           | `framework/areg/base/ResourceListMap.hpp`                      | (same file)                                                      |                                            |
| `ConcurrentResourceMap`               | `LockResourceMap`               | `framework/areg/base/ResourceMap.hpp`                          | (same file)                                                      |                                            |
| `ConcurrentRingStack`                 | `LockRingStack`                 | `framework/areg/base/RingStack.hpp`                            | (same file)                                                      |                                            |
| `ConcurrentRuntimeResourceMap`        | `LockRuntimeResourceMap`        | `framework/areg/base/RuntimeResourceMap.hpp`                   | (same file)                                                      |                                            |
| `ConcurrentStack`                     | `LockStack`                     | `framework/areg/base/Stack.hpp`                                | (same file)                                                      |                                            |
| `OrderedMap`                           | `SortedMap`                     | `framework/areg/base/OrderedMap.hpp`                                  | `framework/areg/base/SortedMap.hpp`                              | `Map` too generic; wraps `std::map`        |
| `ResourceListMap`         | `NolockResourceListMap`         | `framework/areg/base/ResourceListMap.hpp`                      | (same file)                                                      |                                            |
| `ResourceMap`             | `NolockResourceMap`             | `framework/areg/base/ResourceMap.hpp`                          | (same file)                                                      |                                            |
| `RingStack`               | `NolockRingStack`               | `framework/areg/base/RingStack.hpp`                            | (same file)                                                      |                                            |
| `RuntimeResourceMap`      | `NolockRuntimeResourceMap`      | `framework/areg/base/RuntimeResourceMap.hpp`                   | (same file)                                                      |                                            |
| `Stack`                   | `NolockStack`                   | `framework/areg/base/Stack.hpp`                                | (same file)                                                      |                                            |
| `PtrHashMap`                | `PtrHashMap`                    | `framework/areg/base/Containers.hpp`                             | (same file)                                                      | Shorter                                    |
| `PtrMap`                    | `PtrMap`                        | `framework/areg/base/Containers.hpp`                             | (same file)                                                      | Shorter                                    |
| `KeyValuePair`                      | `PropertyPair`                  | `framework/areg/base/KeyValuePair.hpp`                             | `framework/areg/base/PropertyPair.hpp`                           | `Property` exists in persist module        |
| `ResourceListMapBase`               | `ResourceListMap`               | `framework/areg/base/ResourceListMap.hpp`                      | `framework/areg/base/ResourceListMap.hpp`                        |                                            |
| `ResourceListMapImpl`           | `ResourceListMapImpl`           | `framework/areg/base/TemplateBase.hpp`                         | (same file)                                                      |                                            |
| `ResourceMapBase`                   | `ResourceMap`                   | `framework/areg/base/ResourceMap.hpp`                          | `framework/areg/base/ResourceMap.hpp`                            |                                            |
| `ResourceMapImpl`               | `ResourceMapImpl`               | `framework/areg/base/TemplateBase.hpp`                         | (same file)                                                      |                                            |
| `RingStack`                     | `RingStack`                     | `framework/areg/base/RingStack.hpp`                            | `framework/areg/base/RingStack.hpp`                              |                                            |
| `RuntimeHashMap`                | `RuntimeHashMap`                | `framework/areg/base/RuntimeResourceMap.hpp`                   | (same file)                                                      |                                            |
| `RuntimeResourceMapBase`            | `RuntimeResourceMap`            | `framework/areg/base/RuntimeResourceMap.hpp`                   | `framework/areg/base/RuntimeResourceMap.hpp`                     |                                            |
| `SortedLinkedList`              | `SortedLinkedList`              | `framework/areg/base/SortedLinkedList.hpp`                     | `framework/areg/base/SortedLinkedList.hpp`                       |                                            |
| `StackBase`                         | `Stack`                         | `framework/areg/base/Stack.hpp`                                | `framework/areg/base/Stack.hpp`                                  |                                            |
| `StringBase`                        | `StringBase`                    | `framework/areg/base/StringBase.hpp`                               | `framework/areg/base/StringBase.hpp`                             | `String` exists as concrete class          |
| `StringHashMap`                 | `StringHashMap`                 | `framework/areg/base/Containers.hpp`                             | (same file)                                                      |                                            |
| `StringMap`                     | `StringMap`                     | `framework/areg/base/Containers.hpp`                             | (same file)                                                      |                                            |
| `Align`                         | `Align`                         | `framework/areg/base/MemoryDefs.hpp`                               | (same file)                                                      | Struct inside namespace                    |

---

## 3. Structs

Drop `s`, `S_` prefixes. Use PascalCase. Merge inner/alias pairs into one name.

| Old Name (inner / alias)          | New Name                        | File                                                             | Notes                                      |
|-----------------------------------|---------------------------------|------------------------------------------------------------------|--------------------------------------------|
| `BufferHeader` / `BufferHeader`  | `BufferHeader`              | `framework/areg/base/MemoryDefs.hpp`                               | Fix typo: Bufer -> Buffer                  |
| `S_ByteBuffer` / `sByteBuffer`    | `ByteBuffer`                    | `framework/areg/base/MemoryDefs.hpp`                               |                                            |
| `S_RemoteMessage` / `sRemoteMessage`  | `RemoteMessageData`         | `framework/areg/base/MemoryDefs.hpp`                               | `RemoteMessage` exists as class            |
| `S_RemoteMessageHeader` / `sRemoteMessageHeader` | `RemoteMessageHeader` | `framework/areg/base/MemoryDefs.hpp`                          |                                            |
| `S_Rect` / `sRect`                | `Rect`                          | `framework/areg/base/MathDefs.hpp`                                 |                                            |
| `S_Size` / `sSize`                | `Size`                          | `framework/areg/base/MathDefs.hpp`                                 |                                            |
| `S_Point` / `sPoint`              | `Point`                         | `framework/areg/base/MathDefs.hpp`                                 |                                            |
| `S_Coord` / `sCoord`              | `Coord`                         | `framework/areg/base/MathDefs.hpp`                                 |                                            |
| `S_SystemTime` / `sSystemTime`    | `SystemTime`                    | `framework/areg/base/UtilityDefs.hpp`                            |                                            |
| `S_InterfaceData` / `SInterfaceData` | `InterfaceData`              | `framework/areg/component/ServiceDefs.hpp`                         |                                            |
| `sLargeInteger`                   | `LargeInteger`                  | `framework/areg/base/MathDefs.hpp`                                 |                                            |
| `sEntryTypesEnabling`             | `EntryTypesEnabling`            | `framework/areg/appbase/AppDefs.hpp`                       |                                            |
| `sLogInstance`                    | `LogInstance`                   | `framework/areglogger/client/LogObserverApi.h`                   |                                            |
| `sLogMessage`                     | `LogMessageData`                | `framework/areg/logging/LoggingDefs.hpp`                           | `LogMessage` exists as class               |
| `sLogScope`                       | `LogScopeData`                  | `framework/areglogger/client/LogObserverApi.h`                   | `LogScope` exists as class                 |
| `sObserverEvents`                 | `ObserverEvents`                | `framework/areglogger/client/LogObserverApi.h`                   |                                            |
| `sObserverStatus`                 | `ObserverStatus`                | `framework/logobserver/app/LogObserver.hpp`                      |                                            |
| `sOption`                         | `Option`                        | `framework/aregextend/console/OptionParser.hpp`                  |                                            |
| `sOptionSetup`                    | `OptionSetup`                   | `framework/aregextend/console/OptionParser.hpp`                  |                                            |
| `sProperty`                       | `PropertyData`                  | `framework/areg/persist/PersistenceDefs.hpp`                       | `Property` exists as class                 |
| `sPropertyKey`                    | `PropertyKeyData`               | `framework/areg/persist/PersistenceDefs.hpp`                       | `PropertyKey` exists as class              |
| `sScopeFilter`                    | `ScopeFilter`                   | `framework/aregextend/db/LogSqliteDatabase.hpp`                  |                                            |
| `sScopeInfo`                      | `ScopeInfo`                     | `framework/areg/logging/LoggingDefs.hpp`                           |                                            |
| `sServiceConnectedInstance`       | `ServiceConnectedInstance`      | `framework/areg/component/ServiceDefs.hpp`                         |                                            |
| `BufferAllocator`                 | (keep)                          | `framework/areg/base/MemoryDefs.hpp`                               | Already PascalCase, no prefix              |
| `BufferDeleter`                   | (keep)                          | `framework/areg/base/MemoryDefs.hpp`                               | Already PascalCase, no prefix              |
| `Range`                           | (keep)                          | `framework/aregextend/console/OptionParser.hpp`                  | Already PascalCase, no prefix              |

---

## 4. Enums

Drop `e`, `E_` prefixes. Use PascalCase. Merge typedef inner/alias pairs.
Many enums have both `E_Foo` (inner) and `eFoo` (alias) — keep only the clean PascalCase name.

| Old Name (inner / alias)                 | New Name                   | File                                                             | Notes                                      |
|------------------------------------------|----------------------------|------------------------------------------------------------------|--------------------------------------------|
| `E_AppState` / `eApplicationState`       | `AppState`                 | `framework/areg/appbase/AppDefs.hpp`                       |                                            |
| `E_CharDefs` / `eCharDefs`               | `CharDefs`                 | `framework/areg/base/StringDefs.hpp`                               |                                            |
| `E_CompareResult` / `eCompare`           | `CompareResult`            | `framework/areg/base/MathDefs.hpp`                                 |                                            |
| `E_CompletionStatus` / `eCompletionStatus` | `CompletionStatus`       | `framework/areg/base/Thread.hpp`                                 |                                            |
| `E_ConfigKey` / `eConfigKey`             | `ConfigKey`                | `framework/areg/logging/private/LogOptions.hpp`                |                                            |
| `E_ConnectionBehavior` / `eConnectionBehavior` | `ConnectionBehavior`  | `framework/aregextend/service/ServiceCommunicationBase.hpp`       |                                            |
| `E_Cookies` / `eCookies`                 | `CookieId`                 | `framework/areg/base/NECommon.hpp`                               | More descriptive                           |
| `E_CursorPosition` / `eCursorPosition`   | `CursorSeek`               | `framework/areg/base/Cursor.hpp`                       | Avoid collision with class CursorPosition  |
| `E_DataStateType` / `eDataStateType`     | `DataState`                | `framework/areg/component/ServiceDefs.hpp`                         | Shorter                                    |
| `E_DegubPrio` / `DebugPriority`             | `DebugPriority`            | `framework/areg/base/private/DebugDefs.hpp`                        | Fix typo: Degub -> Debug                   |
| `E_DigitSign` / `eDigitSign`             | `DigitSign`                | `framework/areg/base/MathDefs.hpp`                                 |                                            |
| `E_DisconnectReason` / `eDisconnectReason` | `DisconnectReason`       | `framework/areg/component/ServiceDefs.hpp`                         |                                            |
| `E_EventData` / `eEventData`             | `EventDataKind`            | `framework/areg/component/EventDataStream.hpp`                   | `EventData` exists as class                |
| `E_EventFired` / `eEventFired`           | `EventFired`               | `framework/areg/base/private/posix/SyncLockAndWaitPosix.hpp`        |                                            |
| `E_EventOrder` / `eEventOrder`           | `EventOrder`               | `framework/areg/component/private/EventDispatcherBase.hpp`       |                                            |
| `E_EventPriority` / `eEventPriority`     | `EventPriority`            | `framework/areg/component/Event.hpp`                             |                                            |
| `E_EventResetInfo` / `eEventResetInfo`   | `EventResetInfo`           | `framework/areg/base/private/posix/SyncDefsPosix.hpp`            |                                            |
| `E_EventType` / `eEventType`             | `EventType`                | `framework/areg/component/Event.hpp`                             |                                            |
| `E_ExitCodes` / `eExitCodes`             | `ExitCode`                 | `framework/areg/base/ThreadConsumer.hpp`                       | Singular                                   |
| `E_FileOpenBits` / `eFileOpenBits`       | `FileOpenBits`             | `framework/areg/base/FileBase.hpp`                               |                                            |
| `E_FileOpenMode` / `eFileOpenMode`       | `FileOpenMode`             | `framework/areg/base/FileBase.hpp`                               |                                            |
| `E_FuncIdRange` / `eFuncIdRange`         | `FuncIdRange`              | `framework/areg/component/ServiceDefs.hpp`                         |                                            |
| `E_InstanceBitness` / `eInstanceBitness` | `InstanceBitness`          | `framework/areg/component/ServiceDefs.hpp`                         |                                            |
| `E_Layouts` / `eLayouts`                 | `LayoutToken`              | `framework/areg/logging/private/LogOptions.hpp`                | `Layout` too generic                       |
| `E_LogStatus` / `eLogStatus`             | `LogStatus`                | `framework/areg/logging/private/LogOptions.hpp`                |                                            |
| `E_LoggingAction` / `eLoggingAction`     | `LoggingAction`            | `framework/areg/logging/private/LoggingEvent.hpp`                |                                            |
| `E_MatchCondition` / `eMatchCondition`   | `MatchCondition`           | `framework/areg/base/private/posix/SyncDefsPosix.hpp`            |                                            |
| `E_MessageDataType` / `eMessageDataType` | `MessageDataType`          | `framework/areg/component/ServiceDefs.hpp`                         |                                            |
| `E_MessageSource` / `eMessageSource`     | `MessageSource`            | `framework/areg/component/ServiceDefs.hpp`                         |                                            |
| `E_ProcEnv` / `eProcEnv`                 | `ProcessEnv`               | `framework/areg/base/Process.hpp`                                | Clearer                                    |
| `E_Radix` / `eRadix`                     | `Radix`                    | `framework/areg/base/StringDefs.hpp`                               |                                            |
| `E_RequestType` / `eRequestType`         | `RequestType`              | `framework/areg/component/ServiceDefs.hpp`                         |                                            |
| `E_ServiceCall` / `eServiceCalls`        | `ServiceCall`              | `framework/areg/component/ServiceDefs.hpp`                         | Singular                                   |
| `E_ServiceConnection` / `eServiceConnection` | `ServiceConnection`   | `framework/areg/component/ServiceDefs.hpp`                         |                                            |
| `E_ServiceControl` / `eServiceControl`   | `ServiceControl`           | `framework/aregextend/service/SystemServiceBase.hpp`             |                                            |
| `E_ServiceManagerCommands` / `eServiceManagerCommands` | `ServiceManagerCommand` | `framework/areg/component/private/ServiceManagerEvents.hpp` | Singular                               |
| `E_ServiceRequestType` / `eServiceRequestType` | `ServiceRequestType` | `framework/areg/component/ServiceDefs.hpp`                         |                                            |
| `E_ServiceType` / `eServiceType`         | `ServiceType`              | `framework/areg/component/ServiceDefs.hpp`                         |                                            |
| `E_SpecialFolder` / `eSpecialFolder`     | `SpecialFolder`            | `framework/areg/base/File.hpp`                                   |                                            |
| `E_SyncObject` / `eSyncObject`           | `SyncObjectType`           | `framework/areg/base/SyncObject.hpp`                           | `SyncObject` taken by class rename         |
| `E_SyncObjectFired` / `eSyncObjectFired` | `SyncObjectFired`          | `framework/areg/base/private/posix/SyncDefsPosix.hpp`            |                                            |
| `E_ThreadPriority` / `eThreadPriority`   | `ThreadPriority`           | `framework/areg/base/Thread.hpp`                                 |                                            |
| `E_WaitType` / `eWaitType`               | `WaitType`                 | `framework/areg/base/private/posix/SyncLockAndWaitPosix.hpp`        |                                            |
| `ResultType`                             | (keep)                     | `framework/areg/component/ServiceDefs.hpp`                         | Already PascalCase, no prefix              |
| `eBufferType`                            | `BufferType`               | `framework/areg/base/MemoryDefs.hpp`                               |                                            |
| `eColumnType`                            | `ColumnType`               | `framework/aregextend/db/SqliteStatement.hpp`                    |                                            |
| `eConfigKeys`                            | `ConfigKeys`               | `framework/areg/persist/PersistenceDefs.hpp`                       |                                            |
| `eConnectionState`                       | `ConnectionState`          | `framework/areg/ipc/ServiceClientConnectionBase.hpp`             |                                            |
| `eConnectionTypes`                       | `ConnectionType`           | `framework/areg/ipc/RemoteServiceDefs.hpp`                         | Singular                                   |
| `Grouping`                             | `Grouping`                 | `framework/areg/logging/private/ScopeNodeBase.hpp`               | Fix typo: Groupping -> Grouping            |
| `eIndex`                                 | `IndexPosition`            | `framework/areg/base/NECommon.hpp`                               | `Index` too generic                        |
| `eLockedState`                           | `LockedState`              | `framework/areg/base/SyncPrimitives.hpp`                            |                                            |
| `eLogDataType`                           | `LogDataType`              | `framework/areg/logging/LoggingDefs.hpp`                           |                                            |
| `eLogMessageType`                        | `LogMessageType`           | `framework/areg/logging/LoggingDefs.hpp`                           |                                            |
| `eLogPriority`                           | `LogPriority`              | `framework/areg/logging/LoggingDefs.hpp`                           |                                            |
| `eLogType`                               | `LogType`                  | `framework/areglogger/client/LogObserverApi.h`                   |                                            |
| `eLoggerOptions`                         | `LoggerOption`             | `framework/logcollector/app/LogCollector.hpp`                    | Singular                                   |
| `eLoggingAction`                         | `LoggingAction`            | `framework/areg/logging/private/LoggingEvent.hpp`                | (same as E_ alias above)                   |
| `LoggingType`                           | `LoggingType`              | `framework/areg/logging/LoggingDefs.hpp`                           | Fix typo: Loging -> Logging, singular      |
| `eMatchCondition`                        | `MatchCondition`           | `framework/areg/base/private/posix/SyncDefsPosix.hpp`            | (same as E_ alias above)                   |
| `eMessageResult`                         | `MessageResult`            | `framework/areg/base/MemoryDefs.hpp`                               |                                            |
| `eModelState`                            | `ModelState`               | `framework/areg/component/Model.hpp`                        |                                            |
| `eNode`                                  | `NodeType`                 | `framework/areg/logging/private/ScopeNodeBase.hpp`               | `Node` too generic                         |
| `eObserverStates`                        | `ObserverState`            | `framework/areglogger/client/LogObserverApi.h`                   | Singular                                   |
| `eQueryResult`                           | `QueryResult`              | `framework/aregextend/db/SqliteStatement.hpp`                    |                                            |
| `eRemoteConnection`                      | `RemoteConnection`         | `framework/areg/ipc/RemoteServiceDefs.hpp`                         |                                            |
| `eRemoteServices`                        | `RemoteService`            | `framework/areg/ipc/RemoteServiceDefs.hpp`                         | Singular                                   |
| `eRingOverlap`                           | `RingOverlap`              | `framework/areg/base/NECommon.hpp`                               |                                            |
| `eRouterOptions`                         | `RouterOption`             | `framework/mtrouter/app/MultitargetRouter.hpp`                   | Singular                                   |
| `eSendMessage`                           | `SendMessageKind`          | `framework/areg/ipc/SendMessageEvent.hpp`                        | Avoid collision with SendMessageEventData  |
| `eServiceEventCommands`                  | `ServiceEventCommand`      | `framework/areg/ipc/ServiceEvent.hpp`                            | Singular                                   |
| `eServiceOption`                         | `ServiceOption`            | `framework/aregextend/service/SystemServiceDefs.hpp`               |                                            |
| `eSort`                                  | `SortOrder`                | `framework/areg/base/NECommon.hpp`                               | `Sort` too generic                         |
| `eSystemServiceState`                    | `SystemServiceState`       | `framework/aregextend/service/SystemServiceDefs.hpp`               |                                            |
| `eTimerType`                             | `TimerType`                | `framework/areg/component/TimerBase.hpp`                         |                                            |
| `eValidFlags`                            | `ValidFlags`               | `framework/aregextend/console/OptionParser.hpp`                  |                                            |
| `eWaitResult`                            | `WaitResult`               | `framework/areg/base/SyncPrimitives.hpp`                            |                                            |

---

## 5. Namespaces

Per coding rules: namespaces use `snake_case`. Drop `NE` prefix. Keep short and memorable.

| Old Name                          | New Name                        | Primary File                                                     | Notes                                      |
|-----------------------------------|---------------------------------|------------------------------------------------------------------|--------------------------------------------|
| `NEApplication`                   | `app`                           | `framework/areg/appbase/AppDefs.hpp`                       | Short, clear context                       |
| `NECommon`                        | `common`                        | `framework/areg/base/NECommon.hpp`                               |                                            |
| `NEConnection`                    | `connection`                    | `framework/areg/ipc/private/ConnectionDefs.hpp`                    |                                            |
| `NEDebug`                         | `debug`                         | `framework/areg/base/private/DebugDefs.hpp`                        |                                            |
| `NELogCollectorSettings`          | `logcollect`                    | `framework/logcollector/app/LogCollectorNames.hpp`          | Short form                                 |
| `NELogOptions`                    | `logopts`                       | `framework/areg/logging/private/LogOptions.hpp`                | Short form                                 |
| `NELogging`                       | `logging`                       | `framework/areg/logging/LoggingDefs.hpp`                           |                                            |
| `NEMath`                          | `math`                          | `framework/areg/base/MathDefs.hpp`                                 |                                            |
| `NEMemory`                        | `memory`                        | `framework/areg/base/MemoryDefs.hpp`                               |                                            |
| `NEMultitargetRouterSettings`     | `routercfg`                     | `framework/mtrouter/app/MTRouterNames.hpp`         | Short form                                 |
| `NEPersistence`                   | `persist`                       | `framework/areg/persist/PersistenceDefs.hpp`                       | Matches directory name                     |
| `NERegistry`                      | `registry`                      | `framework/areg/component/Model.hpp`                        |                                            |
| `NERemoteService`                 | `remote`                        | `framework/areg/ipc/RemoteServiceDefs.hpp`                         | Short, context is IPC                      |
| `NEService`                       | `service`                       | `framework/areg/component/ServiceDefs.hpp`                         |                                            |
| `NESocket`                        | `socket`                        | `framework/areg/base/SocketDefs.hpp`                               |                                            |
| `NEString`                        | `strutil`                       | `framework/areg/base/StringDefs.hpp`                               | Avoid clash with `String` class            |
| `NESyncTypesIX`                   | `synctypes`                     | `framework/areg/base/private/posix/SyncDefsPosix.hpp`            | Platform-internal                          |
| `NESystemService`                 | `sysservice`                    | `framework/aregextend/service/SystemServiceDefs.hpp`               |                                            |
| `NEUtilities`                     | `util`                          | `framework/areg/base/UtilityDefs.hpp`                            | Short, standard abbreviation               |

---

## 6. Typos Found During Review

| Old (typo)              | Fixed                     | File                                                             |
|-------------------------|---------------------------|------------------------------------------------------------------|
| `BufferHeader`          | `BufferHeader`            | `framework/areg/base/MemoryDefs.hpp`                               |
| `BufferHeader`     | `BufferHeader`            | `framework/areg/base/MemoryDefs.hpp`                               |
| `DayTimeLayout`        | `DayTimeLayout`           | `framework/areg/logging/private/Layouts.hpp`                     |
| `Grouping`            | `Grouping`                | `framework/areg/logging/private/ScopeNodeBase.hpp`               |
| `LoggingType`          | `LoggingType`             | `framework/areg/logging/LoggingDefs.hpp`                           |
| `DebugPriority`            | `DebugPriority`           | `framework/areg/base/private/DebugDefs.hpp`                        |
| `ServiceCommunicationBase` | `ServiceCommunicationBase` | `framework/aregextend/service/ServiceCommunicationBase.hpp`    |

---

## Summary

| Category     | Items to rename | Already clean |
|--------------|-----------------|---------------|
| IE classes   | 32              | 0             |
| TE classes   | 35              | 0             |
| Structs      | 25              | 3             |
| Enums        | ~90 (deduplicated pairs) | 1  |
| Namespaces   | 19              | 0             |
| Typos        | 6               | —             |
| **Total**    | **~207**        |               |
