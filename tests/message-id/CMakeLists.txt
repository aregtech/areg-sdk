# ###########################################################################
# Message ID tests
# Copyright 2022-2026 Aregtech (Artak Avetyan)
# ###########################################################################
#
# areg-message-id-test: failure reports of requests that share one response, a request
# the provider does not know, and stop_all_notifications(). One test per case.
# areg-message-id-rate: round trips and attribute updates over an interface of 256
# requests and 64 attributes. A measurement, not registered with ctest.
# areg-message-identity-test: the generated IDs are the CRC-32C of kind and name, the same
# in OrderForward and OrderReverse, and an extended OrderForward keeps the IDs it shares.
# ###########################################################################

set(AREG_MESSAGE_ID_PROJECT "areg-message-id-test")
set(AREG_MESSAGE_ID_RATE    "areg-message-id-rate")
set(AREG_MESSAGE_IDENTITY   "areg-message-identity-test")
set(AREG_MESSAGE_IDENTITY_EXT "areg-message-identity-ext-test")

addServiceInterface(message_id_generated      tests/message-id/services/SharedResponse.siml)
addServiceInterface(message_id_wide_generated tests/message-id/services/WideService.siml)
addServiceInterface(message_id_forward_generated  tests/message-id/services/OrderForward.siml)
addServiceInterface(message_id_reverse_generated  tests/message-id/services/OrderReverse.siml)
addServiceInterface(message_id_extended_generated tests/message-id/services/extended/OrderForward.siml)

addExecutableEx(${AREG_MESSAGE_ID_PROJECT} ""
    "${AREG_TESTS_DIR}/message-id/message_id_test.cpp"
    "message_id_generated")

addExecutableEx(${AREG_MESSAGE_ID_RATE} ""
    "${AREG_TESTS_DIR}/message-id/wide_rate.cpp"
    "message_id_wide_generated")

addExecutableEx(${AREG_MESSAGE_IDENTITY} ""
    "${AREG_TESTS_DIR}/message-id/message_identity_test.cpp"
    "message_id_forward_generated;message_id_reverse_generated;GTest::gtest_main;GTest::gtest")
gtest_discover_tests(${AREG_MESSAGE_IDENTITY} DISCOVERY_TIMEOUT 60 DISCOVERY_MODE PRE_TEST)

# Only the header of the extended interface is used: a second OrderForward cannot be linked with the first.
addExecutableEx(${AREG_MESSAGE_IDENTITY_EXT} ""
    "${AREG_TESTS_DIR}/message-id/message_identity_extended_test.cpp"
    "message_id_reverse_generated;GTest::gtest_main;GTest::gtest")
add_dependencies(${AREG_MESSAGE_IDENTITY_EXT} message_id_extended_generated)
gtest_discover_tests(${AREG_MESSAGE_IDENTITY_EXT} DISCOVERY_TIMEOUT 60 DISCOVERY_MODE PRE_TEST)

foreach(_msgid_case shared-busy shared-cancel shared-error-request shared-error-response shared-unavailable unknown-request remove-all unsupported-request)
    add_test(NAME ${AREG_MESSAGE_ID_PROJECT}.${_msgid_case} COMMAND ${AREG_MESSAGE_ID_PROJECT} ${_msgid_case})
    set_tests_properties(${AREG_MESSAGE_ID_PROJECT}.${_msgid_case} PROPERTIES TIMEOUT 300)
endforeach()
