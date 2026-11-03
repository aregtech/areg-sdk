# ###########################################################################
# Request error test
# Copyright 2022-2026 Aregtech (Artak Avetyan)
# ###########################################################################
#
# A provider's error of a request reaches its caller as request_X_failed, never as the
# response, and never reaches a consumer that only subscribed to the response.
# Reuses the generated service interface of example 24. One test per case.
# ###########################################################################

set(AREG_REQUEST_ERROR_PROJECT "areg-request-error-test")

addExecutableEx(${AREG_REQUEST_ERROR_PROJECT} ""
    "${AREG_TESTS_DIR}/request-error/request_error_test.cpp"
    "24_generated")

foreach(_error_case error error-unblocked cancel cancel-session response-error error-all subscribed-caller busy)
    add_test(NAME ${AREG_REQUEST_ERROR_PROJECT}.${_error_case} COMMAND ${AREG_REQUEST_ERROR_PROJECT} ${_error_case})
    set_tests_properties(${AREG_REQUEST_ERROR_PROJECT}.${_error_case} PROPERTIES TIMEOUT 300)
endforeach()
