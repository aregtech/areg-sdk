# ###########################################################################
# Consumer delete test
# Copyright 2022-2026 Aregtech (Artak Avetyan)
# ###########################################################################
#
# A client deletes another client of the same proxy, or itself, inside a callback.
# Reuses the generated service interface of example 25. One test per case.
# ###########################################################################

set(AREG_CONSUMER_DELETE_PROJECT "areg-consumer-delete-test")

addExecutableEx(${AREG_CONSUMER_DELETE_PROJECT} ""
    "${AREG_TESTS_DIR}/consumer-delete/consumer_delete_test.cpp"
    "25_generated")

foreach(_consumer_delete_case sibling-initial sibling-update self-delete same-address connect-sibling free-proxy)
    add_test(NAME ${AREG_CONSUMER_DELETE_PROJECT}.${_consumer_delete_case} COMMAND ${AREG_CONSUMER_DELETE_PROJECT} ${_consumer_delete_case})
    set_tests_properties(${AREG_CONSUMER_DELETE_PROJECT}.${_consumer_delete_case} PROPERTIES TIMEOUT 60)
endforeach()
