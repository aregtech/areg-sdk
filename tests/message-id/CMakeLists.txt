# ###########################################################################
# Message ID tests
# Copyright 2022-2026 Aregtech (Artak Avetyan)
# ###########################################################################
#
# areg-message-id-test: failure reports of requests that share one response, a request
# the provider does not know, stop_all_notifications(), and responses with a result no
# provider sends. One test per case.
# areg-message-id-rate: round trips and attribute updates over an interface of 256
# requests and 64 attributes. A measurement, not registered with ctest.
# areg-message-identity-test: the generated IDs are the CRC-32C of kind and name, the same
# in OrderForward and OrderReverse.
# areg-short-payload-test: a request, response, broadcast or attribute update whose payload
# holds fewer bytes than its parameter list, and an error of a broadcast. One test per case.
# areg-kind-share-test: a request and a broadcast with one message ID, and IDs below 0x10000,
# reach their own handlers.
# areg-message-id-rule.same-kind-refused: the code generator refuses two requests, two responses,
# two broadcasts and two attributes with one ID, and accepts a request and a broadcast with one ID.
# ###########################################################################

set(AREG_MESSAGE_ID_PROJECT "areg-message-id-test")
set(AREG_MESSAGE_ID_RATE    "areg-message-id-rate")
set(AREG_MESSAGE_IDENTITY   "areg-message-identity-test")
set(AREG_SHORT_PAYLOAD      "areg-short-payload-test")
set(AREG_KIND_SHARE         "areg-kind-share-test")

addServiceInterface(message_id_generated      tests/message-id/services/SharedResponse.siml)
addServiceInterface(message_id_wide_generated tests/message-id/services/WideService.siml)
addServiceInterface(message_id_forward_generated  tests/message-id/services/OrderForward.siml)
addServiceInterface(message_id_reverse_generated  tests/message-id/services/OrderReverse.siml)
addServiceInterface(message_id_short_generated    tests/message-id/services/ShortPayload.siml)
addServiceInterface(message_id_kind_share_generated tests/message-id/services/KindShare.siml)

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

foreach(_msgid_case shared-busy shared-cancel shared-error-request shared-error-response shared-unavailable unknown-request remove-all unsupported-request foreign-result foreign-failure-on-response)
    add_test(NAME ${AREG_MESSAGE_ID_PROJECT}.${_msgid_case} COMMAND ${AREG_MESSAGE_ID_PROJECT} ${_msgid_case})
    set_tests_properties(${AREG_MESSAGE_ID_PROJECT}.${_msgid_case} PROPERTIES TIMEOUT 300)
endforeach()

addExecutableEx(${AREG_SHORT_PAYLOAD} ""
    "${AREG_TESTS_DIR}/message-id/short_payload_test.cpp"
    "message_id_short_generated")

foreach(_short_case short-request empty-request short-string-request short-response short-broadcast short-attribute broadcast-error)
    add_test(NAME ${AREG_SHORT_PAYLOAD}.${_short_case} COMMAND ${AREG_SHORT_PAYLOAD} ${_short_case})
    set_tests_properties(${AREG_SHORT_PAYLOAD}.${_short_case} PROPERTIES TIMEOUT 300)
endforeach()

addExecutableEx(${AREG_KIND_SHARE} ""
    "${AREG_TESTS_DIR}/message-id/kind_share_test.cpp"
    "message_id_kind_share_generated")

add_test(NAME ${AREG_KIND_SHARE} COMMAND ${AREG_KIND_SHARE})
set_tests_properties(${AREG_KIND_SHARE} PROPERTIES TIMEOUT 300)

add_test(NAME areg-message-id-rule.same-kind-refused
         COMMAND ${Java_JAVA_EXECUTABLE} -jar "${AREG_SDK_TOOLS}/codegen.jar"
                 "--root=${AREG_SDK_ROOT}" "--doc=tests/message-id/rule/SameKindClash.siml"
                 "--target=${CMAKE_CURRENT_BINARY_DIR}/message-id-rule")
set_tests_properties(areg-message-id-rule.same-kind-refused PROPERTIES TIMEOUT 120
    PASS_REGULAR_EXPRESSION "request \\[ value_1371838 \\] and the request \\[ value_2000402 \\].*response \\[ value_1371838 \\] and the response \\[ value_2000402 \\].*broadcast \\[ value_1371838 \\] and the broadcast \\[ value_2000402 \\].*attribute \\[ value_1371838 \\] and the attribute \\[ value_2000402 \\]"
    FAIL_REGULAR_EXPRESSION "pingSDFzY \\]|tickxGUOt \\]")
