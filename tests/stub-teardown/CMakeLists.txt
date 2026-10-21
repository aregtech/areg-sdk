# ###########################################################################
# Stub and proxy teardown test
# Copyright 2022-2026 Aregtech (Artak Avetyan)
# ###########################################################################
#
# Routes service events to providers and consumers from a foreign thread while
# the target is loaded and unloaded, or deleted and created, in a loop.
# One executable, one test per case.
# ###########################################################################

set(AREG_STUB_TEARDOWN_PROJECT "areg-stub-teardown-test")

addExecutableEx(${AREG_STUB_TEARDOWN_PROJECT} ""
    "${AREG_TESTS_DIR}/stub-teardown/stub_teardown_test.cpp"
    "")

foreach(_teardown_case stub-request stub-address proxy-unload proxy-free local-unload local-many local-churn worker-unload)
    add_test(NAME ${AREG_STUB_TEARDOWN_PROJECT}.${_teardown_case} COMMAND ${AREG_STUB_TEARDOWN_PROJECT} ${_teardown_case})
    set_tests_properties(${AREG_STUB_TEARDOWN_PROJECT}.${_teardown_case} PROPERTIES TIMEOUT 180)
endforeach()
