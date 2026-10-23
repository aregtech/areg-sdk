# ###########################################################################
# Router connection I/O test
# Copyright 2022-2026 Aregtech (Artak Avetyan)
# ###########################################################################
#
# Runs the message router service in the test process and drives it with raw
# TCP clients. One executable, one test per case and pool size.
# ###########################################################################

set(AREG_ROUTER_IO_PROJECT "areg-router-io-test")

addExecutableEx(${AREG_ROUTER_IO_PROJECT} ""
    "${AREG_TESTS_DIR}/router-io/router_io_test.cpp;${AREG_FRAMEWORK}/mtrouter/service/private/ListServiceProxies.cpp;${AREG_FRAMEWORK}/mtrouter/service/private/RouterServerService.cpp;${AREG_FRAMEWORK}/mtrouter/service/private/ServiceProxy.cpp;${AREG_FRAMEWORK}/mtrouter/service/private/ServiceRegistry.cpp;${AREG_FRAMEWORK}/mtrouter/service/private/ServiceStub.cpp"
    "")
target_compile_options(${AREG_ROUTER_IO_PROJECT} PRIVATE "${AREG_OPT_DISABLE_WARN_TOOLS}")

foreach(_io_case stale-cache wrong-client partial-header partial-body invalid-type oversize)
    foreach(_io_pairs 0 4)
        add_test(NAME ${AREG_ROUTER_IO_PROJECT}.${_io_case}.${_io_pairs} COMMAND ${AREG_ROUTER_IO_PROJECT} ${_io_case} ${_io_pairs} $<TARGET_FILE_DIR:${AREG_ROUTER_IO_PROJECT}>)
        set_tests_properties(${AREG_ROUTER_IO_PROJECT}.${_io_case}.${_io_pairs} PROPERTIES TIMEOUT 120)
    endforeach()
endforeach()
