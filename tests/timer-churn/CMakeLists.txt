# ###########################################################################
# Timer and watchdog churn test
# Copyright 2022-2026 Aregtech (Artak Avetyan)
# ###########################################################################
#
# Arms, disarms, restarts and destroys timers from several threads while they
# are firing, with the watchdog enabled, and then tears the application down --
# several times over. It is the regression test of the Cygwin teardown hang,
# where the OS ran the expiry on a thread it destroyed itself and a lock was
# left owned by a thread that no longer existed, so
# Application::release() never returned.
#
# It drives the whole application lifecycle and calls Application::setup() and
# Application::release() repeatedly, so it runs as its own executable rather
# than as a case in the unit test binary.
# ###########################################################################

set(AREG_TIMER_CHURN_PROJECT "areg-timer-churn-test")

addExecutableEx(${AREG_TIMER_CHURN_PROJECT} ""
    "${AREG_TESTS_DIR}/timer-churn/timer_churn_test.cpp"
    "")

add_test(NAME ${AREG_TIMER_CHURN_PROJECT} COMMAND ${AREG_TIMER_CHURN_PROJECT})
set_tests_properties(${AREG_TIMER_CHURN_PROJECT} PROPERTIES TIMEOUT 180)
