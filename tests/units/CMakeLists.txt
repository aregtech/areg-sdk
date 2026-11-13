
# ###########################################################################
# Areg Unit tests
# Copyright 2022-2026 Aregtech (Artak Avetyan)
# ###########################################################################

# ---------------------------------------------------------------------------
# Description : Creates an unit test executable from specified list of sources
#               linked with 'areg' and 'gtest' libraries. If the target with the
#               same name exist, the sources are added to the existing executable.
#               Otherwise, the new executable is created using specified source.
# macro ......: macro_add_unit_test
# Parameters .: ${test_project}     -- The name unit test executable.
# Usage ......: macro_add_unit_test( <name of executable> <list of sources>)
# ---------------------------------------------------------------------------
macro(macro_add_unit_test test_project)

    set(_tests)
    set(_list "${ARGN}")
    set(_libs)

    foreach(item IN LISTS _list)
        list(APPEND _tests "${AREG_UNIT_TEST_BASE}/${item}")
    endforeach()

    list(APPEND _libs "GTest::gtest_main" "GTest::gtest")
    addExecutableEx(${test_project} "" "${_tests}" "${_libs}")

    # Fix of Cygwin unit test compilation.
    if (CYGWIN AND CMAKE_BUILD_TYPE MATCHES "Release")
        target_compile_options(${test_project} PRIVATE -O1)
    endif()

    # Use PRE_TEST discovery on macOS, Cygwin, and ARM to avoid build-time binary execution.
    if (APPLE OR CYGWIN OR ${AREG_ARCH} STREQUAL ${_proc_arm32} OR ${AREG_ARCH} STREQUAL ${_proc_arm64})
        gtest_discover_tests(${test_project} DISCOVERY_TIMEOUT 60 DISCOVERY_MODE PRE_TEST)
    else()
        gtest_discover_tests(${test_project} DISCOVERY_TIMEOUT 60)
    endif()

    unset(_tests)
    unset(_list)
    unset(_libs)

endmacro(macro_add_unit_test)

macro_add_unit_test("${AREG_UNIT_TEST_PROJECT}"
    ChildProcessTest.cpp
    ContainersTest.cpp
    MemoryDefsTest.cpp
    GUnitTest.cpp
    ArrayListTest.cpp
    AnsiWriterTest.cpp
    ConsoleTest.cpp
    ConfigManagerTest.cpp
    DateTimeTest.cpp
    DebugDefsTest.cpp
    BufferBaseTest.cpp
    EnvelopeCacheTest.cpp
    EventEnvelopeTest.cpp
    EventQueueTest.cpp
    RouterClientMoreTest.cpp
    FatalErrorTest.cpp
    WatchdogTest.cpp
    RemainingSmallFilesCoverageTest.cpp
    RemainderCoverageTest.cpp
    StringDefsEdgeTest.cpp
    StreamPrimitivesTest.cpp
    StreamTextUnitsTest.cpp
    RuntimeClassIDTest.cpp
    FileBufferTest.cpp
    FileTest.cpp
    FileOsCoverageTest.cpp
    FixedArrayTest.cpp
    EventDefsTest.cpp
    HashMapTest.cpp
    IdentifierTest.cpp
    IpcClientCoverageTest.cpp
    KeyValuePairTest.cpp
    LinkedListTest.cpp
    ListenerDispatchTest.cpp
    LockWaitTest.cpp
    LayoutTest.cpp
    LogCollectorApplicationTest.cpp
    LogCollectorDroppedRowsTest.cpp
    LogCollectorServiceTest.cpp
    LogConfigurationTest.cpp
    LoggingRuntimeCoverageTest.cpp
    LogMessageLengthTest.cpp
    LogProtocolTest.cpp
    LogRingTest.cpp
    LogScopesTest.cpp
    LogSqliteDatabaseTest.cpp
    MapTest.cpp
    ModelTest.cpp
    NameNumberCollisionPeer.cpp
    NameNumberCollisionTest.cpp
    ComponentLoaderContractTest.cpp
    ComponentLifecycleFailureTest.cpp
    ComponentSelfLifecycleTest.cpp
    ProxyBaseContractTest.cpp
    ProxyStubLifecycleTest.cpp
    StubBaseContractTest.cpp
    MultiLockTest.cpp
    NeverDestroyedTest.cpp
    OptionParserTest.cpp
    ProcessTest.cpp
    ApplicationRestartTest.cpp
    ApplicationSetupTest.cpp
    ApplicationSetupPosixTest.cpp
    PropertyTest.cpp
    ReceiveCacheTest.cpp
    ReceivedHeaderTest.cpp
    RingStackTest.cpp
    RouterApplicationTest.cpp
    ServiceConsoleHarness.cpp
    RouterServiceRegistryTest.cpp
    RuntimeObjectTest.cpp
    ScopedLockTest.cpp
    SendBacklogTest.cpp
    ServiceAddressTest.cpp
    ServiceApplicationBaseTest.cpp
    WindowsServiceTest.cpp
    ServiceCommConnectionTest.cpp
    ServiceCommDefsTest.cpp
    ServiceCommServerTest.cpp
    ServiceDefsTest.cpp
    SharedBufferTest.cpp
    SocketAddressTest.cpp
    SocketInterruptTest.cpp
    SocketKeepAliveTest.cpp
    SocketLivenessTest.cpp
    SocketMultiplexerTest.cpp
    SocketOsCoverageTest.cpp
    SocketWouldBlockTest.cpp
    SqliteStatementTest.cpp
    SortedLinkedListTest.cpp
    StreamConverterTest.cpp
    StreamExtractTest.cpp
    StringStreamTest.cpp
    StringUnicodeTest.cpp
    StackTest.cpp
    StringDefsTest.cpp
    StringDefsTest2.cpp
    StringDefsTest3.cpp
    SimpleEventTest.cpp
    StringDefsTest4.cpp 
    StringCommonTest.cpp
    StringTest.cpp
    StringUtilsTest.cpp
    SyncTimerTest.cpp
    SystemServiceBaseTest.cpp
    SystemServiceDefsTest.cpp
    UtilityDefsTest.cpp
    VersionTest.cpp
    ThreadTest.cpp
    ThreadLifecycleCoverageTest.cpp
    SyncPrimitiveCoverageTest.cpp
    MultiLockCoverageTest.cpp
    SyncTimerLifecycleTest.cpp
    WorkerThreadTest.cpp
    DispatcherLifecycleTest.cpp
    TimerLifecycleTest.cpp
    RemoteEventRoutingTest.cpp
)

# The log database tests use the SQLite wrappers of aregextend, which every executable links.
target_link_libraries("${AREG_UNIT_TEST_PROJECT}" PRIVATE ${AREG_SQLITE_LIB_REF})

# The log collector application is not part of a library.
target_sources("${AREG_UNIT_TEST_PROJECT}" PRIVATE
    "${AREG_FRAMEWORK}/logcollector/app/private/LogCollector.cpp"
    "${AREG_FRAMEWORK}/logcollector/app/private/LogCollectorNames.cpp"
    "${AREG_FRAMEWORK}/logcollector/service/private/LogCollectorDatabase.cpp"
    "${AREG_FRAMEWORK}/logcollector/service/private/LogCollectorDroppedRows.cpp"
    "${AREG_FRAMEWORK}/logcollector/service/private/LogCollectorMessageProcessor.cpp"
    "${AREG_FRAMEWORK}/logcollector/service/private/LogCollectorServerService.cpp"
)

# The tests reach private classes of the log observer library, so its sources are built into the executable.
if (AREG_LOGGING)
    target_compile_definitions("${AREG_UNIT_TEST_PROJECT}" PRIVATE IMP_LOGGER_LIB)
    target_sources("${AREG_UNIT_TEST_PROJECT}" PRIVATE
        "${AREG_UNIT_TEST_BASE}/LogObserverApiTest.cpp"
        "${AREG_UNIT_TEST_BASE}/LogObserverAppTest.cpp"
        "${AREG_FRAMEWORK}/areglogger/client/private/LogObserverApi.cpp"
        "${AREG_FRAMEWORK}/areglogger/client/private/LogObserverBase.cpp"
        "${AREG_FRAMEWORK}/areglogger/client/private/LoggerClient.cpp"
        "${AREG_FRAMEWORK}/areglogger/client/private/ObserverMessageProcessor.cpp"
        "${AREG_FRAMEWORK}/logobserver/app/private/LogObserver.cpp"
    )
endif()

# The router application is not part of a library.
target_sources("${AREG_UNIT_TEST_PROJECT}" PRIVATE
    "${AREG_FRAMEWORK}/mtrouter/app/private/MultitargetRouter.cpp"
    "${AREG_FRAMEWORK}/mtrouter/app/private/MTRouterNames.cpp"
    "${AREG_FRAMEWORK}/mtrouter/service/private/RouterServerService.cpp"
)

# The router registry and its value objects are not part of a library.
target_sources("${AREG_UNIT_TEST_PROJECT}" PRIVATE
    "${AREG_FRAMEWORK}/mtrouter/service/private/ListServiceProxies.cpp"
    "${AREG_FRAMEWORK}/mtrouter/service/private/ServiceProxy.cpp"
    "${AREG_FRAMEWORK}/mtrouter/service/private/ServiceRegistry.cpp"
    "${AREG_FRAMEWORK}/mtrouter/service/private/ServiceStub.cpp"
)

# The tests of private POSIX classes are built where the areg library exposes them.
if (NOT WIN32 AND (AREG_LIB_TYPE STREQUAL "static" OR (NOT CYGWIN AND NOT AREG_BUILD_OPTIMIZED)))
    target_compile_definitions("${AREG_UNIT_TEST_PROJECT}" PRIVATE AREG_TEST_PRIVATE_POSIX)
endif()

# Private logging helpers are not exported by the shared library.
if (AREG_LIB_TYPE STREQUAL "shared")
    target_sources("${AREG_UNIT_TEST_PROJECT}" PRIVATE
        "${AREG_FRAMEWORK}/areg/logging/private/Layouts.cpp"
        "${AREG_FRAMEWORK}/areg/logging/private/LayoutManager.cpp"
        "${AREG_FRAMEWORK}/areg/logging/private/ScopeController.cpp"
        "${AREG_FRAMEWORK}/areg/logging/private/ScopeNodeBase.cpp"
        "${AREG_FRAMEWORK}/areg/logging/private/ScopeNodes.cpp"
        "${AREG_FRAMEWORK}/areg/logging/private/LoggingEvent.cpp"
    )
endif()
