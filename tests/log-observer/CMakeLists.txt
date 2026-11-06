# ###########################################################################
# Log observer test
# Copyright 2022-2026 Aregtech (Artak Avetyan)
# ###########################################################################
#
# Drives the log observer C API and LogObserverBase against a real log
# collector. The collector and the log sources run as child processes of the
# test executable. One executable, one test per case.
# ###########################################################################

set(AREG_LOG_OBSERVER_PROJECT "areg-log-observer-test")

if (AREG_LOGGER_LIB_TYPE MATCHES "shared")
    addExecutableEx(${AREG_LOG_OBSERVER_PROJECT} ""
        "${AREG_TESTS_DIR}/log-observer/log_observer_test.cpp"
        "areglogger")
    target_compile_definitions(${AREG_LOG_OBSERVER_PROJECT} PRIVATE IMP_LOGGER_DLL)
else()
    addExecutableEx(${AREG_LOG_OBSERVER_PROJECT} ""
        "${AREG_TESTS_DIR}/log-observer/log_observer_test.cpp"
        "areglogger;${AREG_SQLITE_LIB_REF}")
    target_compile_definitions(${AREG_LOG_OBSERVER_PROJECT} PRIVATE IMP_LOGGER_LIB)
endif()

target_compile_options(${AREG_LOG_OBSERVER_PROJECT} PRIVATE "${AREG_OPT_DISABLE_WARN_TOOLS}")
add_dependencies(${AREG_LOG_OBSERVER_PROJECT} logcollector)

foreach(_observer_case
        calls-before-initialize
        calls-after-release
        offline-lifecycle
        connect-without-collector
        session-messages
        source-leaves
        scope-priority
        source-state
        source-paused-drops-produced-logs
        config-save-restore
        pause-resume
        local-logs-and-database
        reconnect-and-stop
        collector-restart
        raw-message-callback
        invalid-requests
        path-getters-fill-buffer
        config-address-fills-buffer
        observer-base-class)
    add_test(NAME ${AREG_LOG_OBSERVER_PROJECT}.${_observer_case}
             COMMAND ${AREG_LOG_OBSERVER_PROJECT} ${_observer_case} $<TARGET_FILE_DIR:${AREG_LOG_OBSERVER_PROJECT}> $<TARGET_FILE:logcollector>)
    set_tests_properties(${AREG_LOG_OBSERVER_PROJECT}.${_observer_case} PROPERTIES TIMEOUT 300)
endforeach()
