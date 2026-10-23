# ###########################################################################
# Proxy client attach test
# Copyright 2022-2026 Aregtech (Artak Avetyan)
# ###########################################################################

set(AREG_PROXY_ATTACH_PROJECT "areg-proxy-attach-test")

addExecutableEx(${AREG_PROXY_ATTACH_PROJECT} ""
    "${AREG_TESTS_DIR}/proxy-attach/proxy_attach_test.cpp;${AREG_TESTS_DIR}/proxy-attach/attach_provider.cpp"
    "01_generated")

foreach(_attach_case same-thread foreign-thread deleted-before-notify reconnect before-connect)
    add_test(NAME ${AREG_PROXY_ATTACH_PROJECT}.${_attach_case} COMMAND ${AREG_PROXY_ATTACH_PROJECT} ${_attach_case})
    set_tests_properties(${AREG_PROXY_ATTACH_PROJECT}.${_attach_case} PROPERTIES TIMEOUT 60)
endforeach()
