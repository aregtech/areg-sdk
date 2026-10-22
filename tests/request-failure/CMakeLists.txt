# ###########################################################################
# Request failure test
# Copyright 2022-2026 Aregtech (Artak Avetyan)
# ###########################################################################
#
# A request that cannot be answered reaches its caller as a failure: sent before the
# connection, pending when the provider leaves, or refused by the provider queue.
# Reuses the generated service interface of example 02. One test per case.
# ###########################################################################

set(AREG_REQUEST_FAILURE_PROJECT "areg-request-failure-test")

addExecutableEx(${AREG_REQUEST_FAILURE_PROJECT} ""
    "${AREG_TESTS_DIR}/request-failure/request_failure_test.cpp"
    "02_generated")

foreach(_failure_case before-connect provider-gone remote-refused)
    add_test(NAME ${AREG_REQUEST_FAILURE_PROJECT}.${_failure_case} COMMAND ${AREG_REQUEST_FAILURE_PROJECT} ${_failure_case})
    set_tests_properties(${AREG_REQUEST_FAILURE_PROJECT}.${_failure_case} PROPERTIES TIMEOUT 60)
endforeach()
