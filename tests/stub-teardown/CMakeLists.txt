# ###########################################################################
# Stub teardown test
# Copyright 2022-2026 Aregtech (Artak Avetyan)
# ###########################################################################
#
# Routes remote requests to a service provider from a foreign thread while the
# provider is loaded and unloaded in a loop.
# ###########################################################################

set(AREG_STUB_TEARDOWN_PROJECT "areg-stub-teardown-test")

addExecutableEx(${AREG_STUB_TEARDOWN_PROJECT} ""
    "${AREG_TESTS_DIR}/stub-teardown/stub_teardown_test.cpp"
    "")

add_test(NAME ${AREG_STUB_TEARDOWN_PROJECT} COMMAND ${AREG_STUB_TEARDOWN_PROJECT})
set_tests_properties(${AREG_STUB_TEARDOWN_PROJECT} PROPERTIES TIMEOUT 180)
