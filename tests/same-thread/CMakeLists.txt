# ###########################################################################
# Same thread test
# Copyright 2022-2026 Aregtech (Artak Avetyan)
# ###########################################################################
#
# A service provider and its consumer in one component thread: requests and
# responses, and what happens when the queue of that thread is full.
# Reuses the generated service interface of example 02. One test per case.
# ###########################################################################

set(AREG_SAME_THREAD_PROJECT "areg-same-thread-test")

addExecutableEx(${AREG_SAME_THREAD_PROJECT} ""
    "${AREG_TESTS_DIR}/same-thread/same_thread_test.cpp"
    "02_generated")

foreach(_same_thread_case request-reply full-request full-response)
    add_test(NAME ${AREG_SAME_THREAD_PROJECT}.${_same_thread_case} COMMAND ${AREG_SAME_THREAD_PROJECT} ${_same_thread_case})
    set_tests_properties(${AREG_SAME_THREAD_PROJECT}.${_same_thread_case} PROPERTIES TIMEOUT 60)
endforeach()
