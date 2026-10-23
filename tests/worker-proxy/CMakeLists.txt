# ###########################################################################
# Worker thread proxy test
# Copyright 2022-2026 Aregtech (Artak Avetyan)
# ###########################################################################

set(AREG_WORKER_PROXY_PROJECT "areg-worker-proxy-test")

addExecutableEx(${AREG_WORKER_PROXY_PROJECT} ""
    "${AREG_TESTS_DIR}/worker-proxy/worker_proxy_test.cpp;${AREG_TESTS_DIR}/worker-proxy/worker_provider.cpp"
    "33_generated")

foreach(_worker_case heap member on-the-fly late reload)
    add_test(NAME ${AREG_WORKER_PROXY_PROJECT}.${_worker_case} COMMAND ${AREG_WORKER_PROXY_PROJECT} ${_worker_case})
    set_tests_properties(${AREG_WORKER_PROXY_PROJECT}.${_worker_case} PROPERTIES TIMEOUT 60)
endforeach()
