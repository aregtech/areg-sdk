# ###########################################################################
# Router client stop test
# Copyright 2022-2026 Aregtech (Artak Avetyan)
# ###########################################################################
#
# Connects an application to a fake router, then stops the router client in each
# way an application can: release while idle, while flooded with registration
# notifications and while a message is cut, and a disconnect followed by a
# reconnect or by a model load. Each case checks its time limit.
# ###########################################################################

set(AREG_CLIENT_STOP_PROJECT "areg-client-stop-test")

addExecutableEx(${AREG_CLIENT_STOP_PROJECT} ""
    "${AREG_TESTS_DIR}/client-stop/client_stop_test.cpp"
    "")

foreach(_client_stop_case release-idle release-flood release-mid-message disconnect-reconnect disconnect-flood-load)
    add_test(NAME ${AREG_CLIENT_STOP_PROJECT}.${_client_stop_case} COMMAND ${AREG_CLIENT_STOP_PROJECT} ${_client_stop_case})
    set_tests_properties(${AREG_CLIENT_STOP_PROJECT}.${_client_stop_case} PROPERTIES TIMEOUT 60)
endforeach()
