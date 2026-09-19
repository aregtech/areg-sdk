# ###########################################################################
# Internal events sent outside a dispatch and from foreign threads
# Copyright 2022-2026 Aregtech (Artak Avetyan)
# ###########################################################################

set(AREG_INTERNAL_EVENTS_PROJECT "areg-internal-events-test")

addExecutableEx(${AREG_INTERNAL_EVENTS_PROJECT} ""
    "${AREG_TESTS_DIR}/internal-events/internal_events_test.cpp"
    "")

add_test(NAME ${AREG_INTERNAL_EVENTS_PROJECT} COMMAND ${AREG_INTERNAL_EVENTS_PROJECT})
set_tests_properties(${AREG_INTERNAL_EVENTS_PROJECT} PROPERTIES TIMEOUT 120)
