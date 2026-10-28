# ###########################################################################
# System lifecycle test
# Copyright 2022-2026 Aregtech (Artak Avetyan)
# ###########################################################################
#
# Load, unload, the watchdog, the deadlines of start and stop and the failure
# policies of component threads. A case that ends the process runs the
# executable as a child and checks its exit code. One test per case.
# ###########################################################################

set(AREG_SYSTEM_LIFECYCLE_PROJECT "areg-system-lifecycle-test")

addExecutableEx(${AREG_SYSTEM_LIFECYCLE_PROJECT} ""
    "${AREG_TESTS_DIR}/system-lifecycle/system_lifecycle_test.cpp"
    "02_generated")

foreach(_lifecycle_case exit-watchdog restart-watchdog ignore-watchdog unload-restart double-unload start-hang-exit start-hang-ignore start-hang-restart start-hang-load start-self-post stop-hang-exit stop-hang-ignore own-unload release-delete restart-bound load-refusals load-wait logging-toggle user-state)
    add_test(NAME ${AREG_SYSTEM_LIFECYCLE_PROJECT}.${_lifecycle_case} COMMAND ${AREG_SYSTEM_LIFECYCLE_PROJECT} ${_lifecycle_case})
    set_tests_properties(${AREG_SYSTEM_LIFECYCLE_PROJECT}.${_lifecycle_case} PROPERTIES TIMEOUT 60)
endforeach()
