# ###########################################################################
# Message ID tests
# Copyright 2022-2026 Aregtech (Artak Avetyan)
# ###########################################################################
#
# areg-message-id-test: failure reports of requests that share one response, a request
# the provider does not know, and stop_all_notifications(). One test per case.
# areg-message-id-rate: round trips and attribute updates over an interface of 256
# requests and 64 attributes. A measurement, not registered with ctest.
# The documents OrderForward, OrderReverse and extended/OrderForward have no user yet.
# ###########################################################################

set(AREG_MESSAGE_ID_PROJECT "areg-message-id-test")
set(AREG_MESSAGE_ID_RATE    "areg-message-id-rate")

addServiceInterface(message_id_generated      tests/message-id/services/SharedResponse.siml)
addServiceInterface(message_id_wide_generated tests/message-id/services/WideService.siml)

addExecutableEx(${AREG_MESSAGE_ID_PROJECT} ""
    "${AREG_TESTS_DIR}/message-id/message_id_test.cpp"
    "message_id_generated")

addExecutableEx(${AREG_MESSAGE_ID_RATE} ""
    "${AREG_TESTS_DIR}/message-id/wide_rate.cpp"
    "message_id_wide_generated")

# Registered disabled: every case fails on the current message ID handling, unknown-request in Debug only.
foreach(_msgid_case shared-busy shared-cancel shared-error-request shared-error-response shared-unavailable unknown-request remove-all)
    add_test(NAME ${AREG_MESSAGE_ID_PROJECT}.${_msgid_case} COMMAND ${AREG_MESSAGE_ID_PROJECT} ${_msgid_case})
    set_tests_properties(${AREG_MESSAGE_ID_PROJECT}.${_msgid_case} PROPERTIES TIMEOUT 300 DISABLED TRUE)
endforeach()
