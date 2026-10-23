# ###########################################################################
# Foreign thread calls test
# Copyright 2022-2026 Aregtech (Artak Avetyan)
# ###########################################################################

set(AREG_FOREIGN_CALLS_PROJECT "areg-foreign-calls-test")

addExecutableEx(${AREG_FOREIGN_CALLS_PROJECT} ""
    "${AREG_TESTS_DIR}/foreign-calls/foreign_calls_test.cpp;${AREG_TESTS_DIR}/foreign-calls/foreign_provider.cpp"
    "33_generated")

foreach(_foreign_case requests-two-threads provider-set-broadcast provider-response)
    add_test(NAME ${AREG_FOREIGN_CALLS_PROJECT}.${_foreign_case} COMMAND ${AREG_FOREIGN_CALLS_PROJECT} ${_foreign_case})
    set_tests_properties(${AREG_FOREIGN_CALLS_PROJECT}.${_foreign_case} PROPERTIES TIMEOUT 60)
endforeach()
