# ###########################################################################
# Proxy client churn test
# Copyright 2022-2026 Aregtech (Artak Avetyan)
# ###########################################################################
#
# Creates and deletes service clients on the heap while the service goes up and down.
# Reuses the generated service interface of example 01, so it is built only when the
# examples are built.
# ###########################################################################

set(AREG_PROXY_CHURN_PROJECT "areg-proxy-churn-test")

addExecutableEx(${AREG_PROXY_CHURN_PROJECT} ""
    "${AREG_TESTS_DIR}/proxy-churn/proxy_churn_test.cpp;${AREG_TESTS_DIR}/proxy-churn/churn_provider.cpp"
    "01_generated")

add_test(NAME ${AREG_PROXY_CHURN_PROJECT} COMMAND ${AREG_PROXY_CHURN_PROJECT})
set_tests_properties(${AREG_PROXY_CHURN_PROJECT} PROPERTIES TIMEOUT 180)
