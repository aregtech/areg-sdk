# ###########################################################################
# Client link test
# Copyright 2022-2026 Aregtech (Artak Avetyan)
# ###########################################################################
#
# Builds a client of the areg library without link time optimization, as an
# application of a user usually is, so that a call the compiler devirtualizes
# must find its target. Fails to link when an exported class of areg has an
# inline virtual method (bug 418); runs and checks the locks when it links.
# ###########################################################################

set(AREG_CLIENT_LINK_PROJECT "areg-client-link-test")

addExecutableEx(${AREG_CLIENT_LINK_PROJECT} ""
    "${AREG_TESTS_DIR}/client-link/client_link_test.cpp"
    "")

set_target_properties(${AREG_CLIENT_LINK_PROJECT} PROPERTIES INTERPROCEDURAL_OPTIMIZATION OFF)

add_test(NAME ${AREG_CLIENT_LINK_PROJECT} COMMAND ${AREG_CLIENT_LINK_PROJECT})
set_tests_properties(${AREG_CLIENT_LINK_PROJECT} PROPERTIES TIMEOUT 60)
