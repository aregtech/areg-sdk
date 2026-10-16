# ###########################################################################
# Fail-fast test of a lock the system cannot create (Linux)
# Copyright 2022-2026 Aregtech (Artak Avetyan)
# ###########################################################################
#
# Replaces pthread_mutex_init() in its own executable to make it fail, and checks
# that creating an areg::Mutex then ends the process through areg::fatal_error()
# instead of handing out a lock that cannot lock.
# ###########################################################################

set(AREG_FATAL_CREATE_PROJECT "areg-fatal-create-test")

addExecutableEx(${AREG_FATAL_CREATE_PROJECT} ""
    "${AREG_TESTS_DIR}/fatal-create/fatal_create_test.cpp"
    "${CMAKE_DL_LIBS}")

add_test(NAME ${AREG_FATAL_CREATE_PROJECT} COMMAND ${AREG_FATAL_CREATE_PROJECT})
set_tests_properties(${AREG_FATAL_CREATE_PROJECT} PROPERTIES TIMEOUT 60)
