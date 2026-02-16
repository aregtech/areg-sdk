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
| `Lockable`                  | `ResourceLockBase`              | `framework/areg/base/SyncObjects.hpp`                            | (same file)                                                      | `ResourceLock` exists as concrete class    |
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
| `TEArrayList`                     | `ArrayList`                     | `framework/areg/base/TEArrayList.hpp`                            | `framework/areg/base/ArrayList.hpp`                              |                                            |
| `TEFixedArray`                    | `FixedArray`                    | `framework/areg/base/TEFixedArray.hpp`                           | `framework/areg/base/FixedArray.hpp`                             |                                            |
| `TEHashMap`                       | `HashMap`                       | `framework/areg/base/TEHashMap.hpp`                              | `framework/areg/base/HashMap.hpp`                                |                                            |
| `TEIdHashMap`                     | `IdHashMap`                     | `framework/areg/base/Containers.hpp`                             | (same file)                                                      |                                            |
| `TEIdMap`                         | `IdMap`                         | `framework/areg/base/Containers.hpp`                             | (same file)                                                      |                                            |
| `TEIntegerHashMap`                | `IntHashMap`                    | `framework/areg/base/Containers.hpp`                             | (same file)                                                      | Shorter                                    |
| `TEIntegerMap`                    | `IntMap`                        | `framework/areg/base/Containers.hpp`                             | (same file)                                                      | Shorter                                    |
| `TELinkedList`                    | `LinkedList`                    | `framework/areg/base/TELinkedList.hpp`                           | `framework/areg/base/LinkedList.hpp`                             |                                            |
| `TELockResourceListMap`           | `LockResourceListMap`           | `framework/areg/base/TEResourceListMap.hpp`                      | (same file)                                                      |                                            |
| `TELockResourceMap`               | `LockResourceMap`               | `framework/areg/base/TEResourceMap.hpp`                          | (same file)                                                      |                                            |
| `TELockRingStack`                 | `LockRingStack`                 | `framework/areg/base/TERingStack.hpp`                            | (same file)                                                      |                                            |
| `TELockRuntimeResourceMap`        | `LockRuntimeResourceMap`        | `framework/areg/base/TERuntimeResourceMap.hpp`                   | (same file)                                                      |                                            |
| `TELockStack`                     | `LockStack`                     | `framework/areg/base/TEStack.hpp`                                | (same file)                                                      |                                            |
| `TEMap`                           | `SortedMap`                     | `framework/areg/base/TEMap.hpp`                                  | `framework/areg/base/SortedMap.hpp`                              | `Map` too generic; wraps `std::map`        |
| `TENolockResourceListMap`         | `NolockResourceListMap`         | `framework/areg/base/TEResourceListMap.hpp`                      | (same file)                                                      |                                            |
| `TENolockResourceMap`             | `NolockResourceMap`             | `framework/areg/base/TEResourceMap.hpp`                          | (same file)                                                      |                                            |
| `TENolockRingStack`               | `NolockRingStack`               | `framework/areg/base/TERingStack.hpp`                            | (same file)                                                      |                                            |
| `TENolockRuntimeResourceMap`      | `NolockRuntimeResourceMap`      | `framework/areg/base/TERuntimeResourceMap.hpp`                   | (same file)                                                      |                                            |
| `TENolockStack`                   | `NolockStack`                   | `framework/areg/base/TEStack.hpp`                                | (same file)                                                      |                                            |
| `TEObjectFactory`                 | `ObjectFactory`                 | `framework/areg/base/ObjectBase.hpp`                        | (same file, moves with GenericObject)                            |                                            |
| `TEPointerHashMap`                | `PtrHashMap`                    | `framework/areg/base/Containers.hpp`                             | (same file)                                                      | Shorter                                    |
| `TEPointerMap`                    | `PtrMap`                        | `framework/areg/base/Containers.hpp`                             | (same file)                                                      | Shorter                                    |
| `TEProperty`                      | `PropertyPair`                  | `framework/areg/base/TEProperty.hpp`                             | `framework/areg/base/PropertyPair.hpp`                           | `Property` exists in persist module        |
| `TEResourceListMap`               | `ResourceListMap`               | `framework/areg/base/TEResourceListMap.hpp`                      | `framework/areg/base/ResourceListMap.hpp`                        |                                            |
| `TEResourceListMapImpl`           | `ResourceListMapImpl`           | `framework/areg/base/TETemplateBase.hpp`                         | (same file)                                                      |                                            |
| `TEResourceMap`                   | `ResourceMap`                   | `framework/areg/base/TEResourceMap.hpp`                          | `framework/areg/base/ResourceMap.hpp`                            |                                            |
| `TEResourceMapImpl`               | `ResourceMapImpl`               | `framework/areg/base/TETemplateBase.hpp`                         | (same file)                                                      |                                            |
| `TERingStack`                     | `RingStack`                     | `framework/areg/base/TERingStack.hpp`                            | `framework/areg/base/RingStack.hpp`                              |                                            |
| `TERuntimeHashMap`                | `RuntimeHashMap`                | `framework/areg/base/TERuntimeResourceMap.hpp`                   | (same file)                                                      |                                            |
| `TERuntimeResourceMap`            | `RuntimeResourceMap`            | `framework/areg/base/TERuntimeResourceMap.hpp`                   | `framework/areg/base/RuntimeResourceMap.hpp`                     |                                            |
| `TESortedLinkedList`              | `SortedLinkedList`              | `framework/areg/base/TESortedLinkedList.hpp`                     | `framework/areg/base/SortedLinkedList.hpp`                       |                                            |
| `TEStack`                         | `Stack`                         | `framework/areg/base/TEStack.hpp`                                | `framework/areg/base/Stack.hpp`                                  |                                            |
| `TEString`                        | `StringBase`                    | `framework/areg/base/TEString.hpp`                               | `framework/areg/base/StringBase.hpp`                             | `String` exists as concrete class          |
| `TEStringHashMap`                 | `StringHashMap`                 | `framework/areg/base/Containers.hpp`                             | (same file)                                                      |                                            |
| `TEStringMap`                     | `StringMap`                     | `framework/areg/base/Containers.hpp`                             | (same file)                                                      |                                            |
| `TEAlign`                         | `Align`                         | `framework/areg/base/NEMemory.hpp`                               | (same file)                                                      | Struct inside namespace                    |

---

## 3. Structs

Drop `s`, `S_` prefixes. Use PascalCase. Merge inner/alias pairs into one name.

| Old Name (inner / alias)          | New Name                        | File                                                             | Notes                                      |
|-----------------------------------|---------------------------------|------------------------------------------------------------------|--------------------------------------------|
| `S_BuferHeaderInfo` / `sBuferHeader`  | `BufferHeader`              | `framework/areg/base/NEMemory.hpp`                               | Fix typo: Bufer -> Buffer                  |
| `S_ByteBuffer` / `sByteBuffer`    | `ByteBuffer`                    | `framework/areg/base/NEMemory.hpp`                               |                                            |
| `S_RemoteMessage` / `sRemoteMessage`  | `RemoteMessageData`         | `framework/areg/base/NEMemory.hpp`                               | `RemoteMessage` exists as class            |
| `S_RemoteMessageHeader` / `sRemoteMessageHeader` | `RemoteMessageHeader` | `framework/areg/base/NEMemory.hpp`                          |                                            |
| `S_Rect` / `sRect`                | `Rect`                          | `framework/areg/base/NEMath.hpp`                                 |                                            |
| `S_Size` / `sSize`                | `Size`                          | `framework/areg/base/NEMath.hpp`                                 |                                            |
| `S_Point` / `sPoint`              | `Point`                         | `framework/areg/base/NEMath.hpp`                                 |                                            |
| `S_Coord` / `sCoord`              | `Coord`                         | `framework/areg/base/NEMath.hpp`                                 |                                            |
| `S_SystemTime` / `sSystemTime`    | `SystemTime`                    | `framework/areg/base/NEUtilities.hpp`                            |                                            |
| `S_InterfaceData` / `SInterfaceData` | `InterfaceData`              | `framework/areg/component/NEService.hpp`                         |                                            |
| `sLargeInteger`                   | `LargeInteger`                  | `framework/areg/base/NEMath.hpp`                                 |                                            |
| `sEntryTypesEnabling`             | `EntryTypesEnabling`            | `framework/areg/appbase/NEApplication.hpp`                       |                                            |
| `sLogInstance`                    | `LogInstance`                   | `framework/areglogger/client/LogObserverApi.h`                   |                                            |
| `sLogMessage`                     | `LogMessageData`                | `framework/areg/logging/NELogging.hpp`                           | `LogMessage` exists as class               |
| `sLogScope`                       | `LogScopeData`                  | `framework/areglogger/client/LogObserverApi.h`                   | `LogScope` exists as class                 |
| `sObserverEvents`                 | `ObserverEvents`                | `framework/areglogger/client/LogObserverApi.h`                   |                                            |
| `sObserverStatus`                 | `ObserverStatus`                | `framework/logobserver/app/LogObserver.hpp`                      |                                            |
| `sOption`                         | `Option`                        | `framework/aregextend/console/OptionParser.hpp`                  |                                            |
| `sOptionSetup`                    | `OptionSetup`                   | `framework/aregextend/console/OptionParser.hpp`                  |                                            |
| `sProperty`                       | `PropertyData`                  | `framework/areg/persist/NEPersistence.hpp`                       | `Property` exists as class                 |
| `sPropertyKey`                    | `PropertyKeyData`               | `framework/areg/persist/NEPersistence.hpp`                       | `PropertyKey` exists as class              |
| `sScopeFilter`                    | `ScopeFilter`                   | `framework/aregextend/db/LogSqliteDatabase.hpp`                  |                                            |
| `sScopeInfo`                      | `ScopeInfo`                     | `framework/areg/logging/NELogging.hpp`                           |                                            |
| `sServiceConnectedInstance`       | `ServiceConnectedInstance`      | `framework/areg/component/NEService.hpp`                         |                                            |
| `BufferAllocator`                 | (keep)                          | `framework/areg/base/NEMemory.hpp`                               | Already PascalCase, no prefix              |
| `BufferDeleter`                   | (keep)                          | `framework/areg/base/NEMemory.hpp`                               | Already PascalCase, no prefix              |
| `Range`                           | (keep)                          | `framework/aregextend/console/OptionParser.hpp`                  | Already PascalCase, no prefix              |

---

## 4. Enums

Drop `e`, `E_` prefixes. Use PascalCase. Merge typedef inner/alias pairs.
Many enums have both `E_Foo` (inner) and `eFoo` (alias) — keep only the clean PascalCase name.

| Old Name (inner / alias)                 | New Name                   | File                                                             | Notes                                      |
|------------------------------------------|----------------------------|------------------------------------------------------------------|--------------------------------------------|
| `E_AppState` / `eApplicationState`       | `AppState`                 | `framework/areg/appbase/NEApplication.hpp`                       |                                            |
| `E_CharDefs` / `eCharDefs`               | `CharDefs`                 | `framework/areg/base/NEString.hpp`                               |                                            |
| `E_CompareResult` / `eCompare`           | `CompareResult`            | `framework/areg/base/NEMath.hpp`                                 |                                            |
| `E_CompletionStatus` / `eCompletionStatus` | `CompletionStatus`       | `framework/areg/base/Thread.hpp`                                 |                                            |
| `E_ConfigKey` / `eConfigKey`             | `ConfigKey`                | `framework/areg/logging/private/NELogOptions.hpp`                |                                            |
| `E_ConnectionBehavior` / `eConnectionBehavior` | `ConnectionBehavior`  | `framework/aregextend/service/ServiceCommunicatonBase.hpp`       |                                            |
| `E_Cookies` / `eCookies`                 | `CookieId`                 | `framework/areg/base/NECommon.hpp`                               | More descriptive                           |
| `E_CursorPosition` / `eCursorPosition`   | `CursorSeek`               | `framework/areg/base/Cursor.hpp`                       | Avoid collision with class CursorPosition  |
| `E_DataStateType` / `eDataStateType`     | `DataState`                | `framework/areg/component/NEService.hpp`                         | Shorter                                    |
| `E_DegubPrio` / `eDegubPrio`             | `DebugPriority`            | `framework/areg/base/private/NEDebug.hpp`                        | Fix typo: Degub -> Debug                   |
| `E_DigitSign` / `eDigitSign`             | `DigitSign`                | `framework/areg/base/NEMath.hpp`                                 |                                            |
| `E_DisconnectReason` / `eDisconnectReason` | `DisconnectReason`       | `framework/areg/component/NEService.hpp`                         |                                            |
| `E_EventData` / `eEventData`             | `EventDataKind`            | `framework/areg/component/EventDataStream.hpp`                   | `EventData` exists as class                |
| `E_EventFired` / `eEventFired`           | `EventFired`               | `framework/areg/base/private/posix/SyncLockAndWaitPosix.hpp`        |                                            |
| `E_EventOrder` / `eEventOrder`           | `EventOrder`               | `framework/areg/component/private/EventDispatcherBase.hpp`       |                                            |
| `E_EventPriority` / `eEventPriority`     | `EventPriority`            | `framework/areg/component/Event.hpp`                             |                                            |
| `E_EventResetInfo` / `eEventResetInfo`   | `EventResetInfo`           | `framework/areg/base/private/posix/NESyncTypesIX.hpp`            |                                            |
| `E_EventType` / `eEventType`             | `EventType`                | `framework/areg/component/Event.hpp`                             |                                            |
| `E_ExitCodes` / `eExitCodes`             | `ExitCode`                 | `framework/areg/base/ThreadConsumer.hpp`                       | Singular                                   |
| `E_FileOpenBits` / `eFileOpenBits`       | `FileOpenBits`             | `framework/areg/base/FileBase.hpp`                               |                                            |
| `E_FileOpenMode` / `eFileOpenMode`       | `FileOpenMode`             | `framework/areg/base/FileBase.hpp`                               |                                            |
| `E_FuncIdRange` / `eFuncIdRange`         | `FuncIdRange`              | `framework/areg/component/NEService.hpp`                         |                                            |
| `E_InstanceBitness` / `eInstanceBitness` | `InstanceBitness`          | `framework/areg/component/NEService.hpp`                         |                                            |
| `E_Layouts` / `eLayouts`                 | `LayoutToken`              | `framework/areg/logging/private/NELogOptions.hpp`                | `Layout` too generic                       |
| `E_LogStatus` / `eLogStatus`             | `LogStatus`                | `framework/areg/logging/private/NELogOptions.hpp`                |                                            |
| `E_LoggingAction` / `eLoggingAction`     | `LoggingAction`            | `framework/areg/logging/private/LoggingEvent.hpp`                |                                            |
| `E_MatchCondition` / `eMatchCondition`   | `MatchCondition`           | `framework/areg/base/private/posix/NESyncTypesIX.hpp`            |                                            |
| `E_MessageDataType` / `eMessageDataType` | `MessageDataType`          | `framework/areg/component/NEService.hpp`                         |                                            |
| `E_MessageSource` / `eMessageSource`     | `MessageSource`            | `framework/areg/component/NEService.hpp`                         |                                            |
| `E_ProcEnv` / `eProcEnv`                 | `ProcessEnv`               | `framework/areg/base/Process.hpp`                                | Clearer                                    |
| `E_Radix` / `eRadix`                     | `Radix`                    | `framework/areg/base/NEString.hpp`                               |                                            |
| `E_RequestType` / `eRequestType`         | `RequestType`              | `framework/areg/component/NEService.hpp`                         |                                            |
| `E_ServiceCall` / `eServiceCalls`        | `ServiceCall`              | `framework/areg/component/NEService.hpp`                         | Singular                                   |
| `E_ServiceConnection` / `eServiceConnection` | `ServiceConnection`   | `framework/areg/component/NEService.hpp`                         |                                            |
| `E_ServiceControl` / `eServiceControl`   | `ServiceControl`           | `framework/aregextend/service/SystemServiceBase.hpp`             |                                            |
| `E_ServiceManagerCommands` / `eServiceManagerCommands` | `ServiceManagerCommand` | `framework/areg/component/private/ServiceManagerEvents.hpp` | Singular                               |
| `E_ServiceRequestType` / `eServiceRequestType` | `ServiceRequestType` | `framework/areg/component/NEService.hpp`                         |                                            |
| `E_ServiceType` / `eServiceType`         | `ServiceType`              | `framework/areg/component/NEService.hpp`                         |                                            |
| `E_SpecialFolder` / `eSpecialFolder`     | `SpecialFolder`            | `framework/areg/base/File.hpp`                                   |                                            |
| `E_SyncObject` / `eSyncObject`           | `SyncObjectType`           | `framework/areg/base/SyncObject.hpp`                           | `SyncObject` taken by class rename         |
| `E_SyncObjectFired` / `eSyncObjectFired` | `SyncObjectFired`          | `framework/areg/base/private/posix/NESyncTypesIX.hpp`            |                                            |
| `E_ThreadPriority` / `eThreadPriority`   | `ThreadPriority`           | `framework/areg/base/Thread.hpp`                                 |                                            |
| `E_WaitType` / `eWaitType`               | `WaitType`                 | `framework/areg/base/private/posix/SyncLockAndWaitPosix.hpp`        |                                            |
| `ResultType`                             | (keep)                     | `framework/areg/component/NEService.hpp`                         | Already PascalCase, no prefix              |
| `eBufferType`                            | `BufferType`               | `framework/areg/base/NEMemory.hpp`                               |                                            |
| `eColumnType`                            | `ColumnType`               | `framework/aregextend/db/SqliteStatement.hpp`                    |                                            |
| `eConfigKeys`                            | `ConfigKeys`               | `framework/areg/persist/NEPersistence.hpp`                       |                                            |
| `eConnectionState`                       | `ConnectionState`          | `framework/areg/ipc/ServiceClientConnectionBase.hpp`             |                                            |
| `eConnectionTypes`                       | `ConnectionType`           | `framework/areg/ipc/NERemoteService.hpp`                         | Singular                                   |
| `eGroupping`                             | `Grouping`                 | `framework/areg/logging/private/ScopeNodeBase.hpp`               | Fix typo: Groupping -> Grouping            |
| `eIndex`                                 | `IndexPosition`            | `framework/areg/base/NECommon.hpp`                               | `Index` too generic                        |
| `eLockedState`                           | `LockedState`              | `framework/areg/base/SyncObjects.hpp`                            |                                            |
| `eLogDataType`                           | `LogDataType`              | `framework/areg/logging/NELogging.hpp`                           |                                            |
| `eLogMessageType`                        | `LogMessageType`           | `framework/areg/logging/NELogging.hpp`                           |                                            |
| `eLogPriority`                           | `LogPriority`              | `framework/areg/logging/NELogging.hpp`                           |                                            |
| `eLogType`                               | `LogType`                  | `framework/areglogger/client/LogObserverApi.h`                   |                                            |
| `eLoggerOptions`                         | `LoggerOption`             | `framework/logcollector/app/LogCollector.hpp`                    | Singular                                   |
| `eLoggingAction`                         | `LoggingAction`            | `framework/areg/logging/private/LoggingEvent.hpp`                | (same as E_ alias above)                   |
| `eLogingTypes`                           | `LoggingType`              | `framework/areg/logging/NELogging.hpp`                           | Fix typo: Loging -> Logging, singular      |
| `eMatchCondition`                        | `MatchCondition`           | `framework/areg/base/private/posix/NESyncTypesIX.hpp`            | (same as E_ alias above)                   |
| `eMessageResult`                         | `MessageResult`            | `framework/areg/base/NEMemory.hpp`                               |                                            |
| `eModelState`                            | `ModelState`               | `framework/areg/component/NERegistry.hpp`                        |                                            |
| `eNode`                                  | `NodeType`                 | `framework/areg/logging/private/ScopeNodeBase.hpp`               | `Node` too generic                         |
| `eObserverStates`                        | `ObserverState`            | `framework/areglogger/client/LogObserverApi.h`                   | Singular                                   |
| `eQueryResult`                           | `QueryResult`              | `framework/aregextend/db/SqliteStatement.hpp`                    |                                            |
| `eRemoteConnection`                      | `RemoteConnection`         | `framework/areg/ipc/NERemoteService.hpp`                         |                                            |
| `eRemoteServices`                        | `RemoteService`            | `framework/areg/ipc/NERemoteService.hpp`                         | Singular                                   |
| `eRingOverlap`                           | `RingOverlap`              | `framework/areg/base/NECommon.hpp`                               |                                            |
| `eRouterOptions`                         | `RouterOption`             | `framework/mtrouter/app/MultitargetRouter.hpp`                   | Singular                                   |
| `eSendMessage`                           | `SendMessageKind`          | `framework/areg/ipc/SendMessageEvent.hpp`                        | Avoid collision with SendMessageEventData  |
| `eServiceEventCommands`                  | `ServiceEventCommand`      | `framework/areg/ipc/ServiceEvent.hpp`                            | Singular                                   |
| `eServiceOption`                         | `ServiceOption`            | `framework/aregextend/service/NESystemService.hpp`               |                                            |
| `eSort`                                  | `SortOrder`                | `framework/areg/base/NECommon.hpp`                               | `Sort` too generic                         |
| `eSystemServiceState`                    | `SystemServiceState`       | `framework/aregextend/service/NESystemService.hpp`               |                                            |
| `eTimerType`                             | `TimerType`                | `framework/areg/component/TimerBase.hpp`                         |                                            |
| `eValidFlags`                            | `ValidFlags`               | `framework/aregextend/console/OptionParser.hpp`                  |                                            |
| `eWaitResult`                            | `WaitResult`               | `framework/areg/base/SyncObjects.hpp`                            |                                            |

---

## 5. Namespaces

Per coding rules: namespaces use `snake_case`. Drop `NE` prefix. Keep short and memorable.

| Old Name                          | New Name                        | Primary File                                                     | Notes                                      |
|-----------------------------------|---------------------------------|------------------------------------------------------------------|--------------------------------------------|
| `NEApplication`                   | `app`                           | `framework/areg/appbase/NEApplication.hpp`                       | Short, clear context                       |
| `NECommon`                        | `common`                        | `framework/areg/base/NECommon.hpp`                               |                                            |
| `NEConnection`                    | `connection`                    | `framework/areg/ipc/private/NEConnection.hpp`                    |                                            |
| `NEDebug`                         | `debug`                         | `framework/areg/base/private/NEDebug.hpp`                        |                                            |
| `NELogCollectorSettings`          | `logcollect`                    | `framework/logcollector/app/NELogCollectorSettings.hpp`          | Short form                                 |
| `NELogOptions`                    | `logopts`                       | `framework/areg/logging/private/NELogOptions.hpp`                | Short form                                 |
| `NELogging`                       | `logging`                       | `framework/areg/logging/NELogging.hpp`                           |                                            |
| `NEMath`                          | `math`                          | `framework/areg/base/NEMath.hpp`                                 |                                            |
| `NEMemory`                        | `memory`                        | `framework/areg/base/NEMemory.hpp`                               |                                            |
| `NEMultitargetRouterSettings`     | `routercfg`                     | `framework/mtrouter/app/NEMultitargetRouterSettings.hpp`         | Short form                                 |
| `NEPersistence`                   | `persist`                       | `framework/areg/persist/NEPersistence.hpp`                       | Matches directory name                     |
| `NERegistry`                      | `registry`                      | `framework/areg/component/NERegistry.hpp`                        |                                            |
| `NERemoteService`                 | `remote`                        | `framework/areg/ipc/NERemoteService.hpp`                         | Short, context is IPC                      |
| `NEService`                       | `service`                       | `framework/areg/component/NEService.hpp`                         |                                            |
| `NESocket`                        | `socket`                        | `framework/areg/base/NESocket.hpp`                               |                                            |
| `NEString`                        | `strutil`                       | `framework/areg/base/NEString.hpp`                               | Avoid clash with `String` class            |
| `NESyncTypesIX`                   | `synctypes`                     | `framework/areg/base/private/posix/NESyncTypesIX.hpp`            | Platform-internal                          |
| `NESystemService`                 | `sysservice`                    | `framework/aregextend/service/NESystemService.hpp`               |                                            |
| `NEUtilities`                     | `util`                          | `framework/areg/base/NEUtilities.hpp`                            | Short, standard abbreviation               |

---

## 6. Typos Found During Review

| Old (typo)              | Fixed                     | File                                                             |
|-------------------------|---------------------------|------------------------------------------------------------------|
| `sBuferHeader`          | `BufferHeader`            | `framework/areg/base/NEMemory.hpp`                               |
| `S_BuferHeaderInfo`     | `BufferHeader`            | `framework/areg/base/NEMemory.hpp`                               |
| `DayTimeLaytout`        | `DayTimeLayout`           | `framework/areg/logging/private/Layouts.hpp`                     |
| `eGroupping`            | `Grouping`                | `framework/areg/logging/private/ScopeNodeBase.hpp`               |
| `eLogingTypes`          | `LoggingType`             | `framework/areg/logging/NELogging.hpp`                           |
| `eDegubPrio`            | `DebugPriority`           | `framework/areg/base/private/NEDebug.hpp`                        |
| `ServiceCommunicatonBase` | `ServiceCommunicationBase` | `framework/aregextend/service/ServiceCommunicatonBase.hpp`    |

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
