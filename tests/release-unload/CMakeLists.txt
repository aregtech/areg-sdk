# ###########################################################################
# Release without unload test
# Copyright 2022-2026 Aregtech (Artak Avetyan)
# ###########################################################################
#
# Loads a model, then calls Application::release() without calling
# Application::unload_model() first, and checks that the component threads are
# joined and the components deleted.
#
# It is a separate executable rather than a case in the unit test binary,
# because Application::setup() and Application::release() act on the whole
# process and run once, so it cannot share a process with the other tests.
# ###########################################################################

set(AREG_RELEASE_UNLOAD_PROJECT "areg-release-unload-test")

addExecutableEx(${AREG_RELEASE_UNLOAD_PROJECT} ""
    "${AREG_TESTS_DIR}/release-unload/release_unload_test.cpp"
    "")

add_test(NAME ${AREG_RELEASE_UNLOAD_PROJECT} COMMAND ${AREG_RELEASE_UNLOAD_PROJECT})
set_tests_properties(${AREG_RELEASE_UNLOAD_PROJECT} PROPERTIES TIMEOUT 120)
