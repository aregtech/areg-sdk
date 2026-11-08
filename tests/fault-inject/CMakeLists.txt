# ###########################################################################
# Tests of the paths taken when a system call fails
# Copyright 2022-2026 Aregtech (Artak Avetyan)
# ###########################################################################
#
# areg-fault-inject:      a shared library that makes chosen system calls fail on request
#                         (see FaultInject.hpp). It does not link areg.
# areg-fault-inject-test: googletest cases that arm a fault in a child process and check
#                         how areg reports the failure.
# ###########################################################################

set(AREG_FAULT_INJECT_LIB  "areg-fault-inject")
set(AREG_FAULT_INJECT_TEST "areg-fault-inject-test")

add_library(${AREG_FAULT_INJECT_LIB} SHARED "${AREG_TESTS_DIR}/fault-inject/fault_inject.cpp")
target_compile_options(${AREG_FAULT_INJECT_LIB} PRIVATE "${AREG_OPT_DISABLE_WARN_COMMON}")
target_include_directories(${AREG_FAULT_INJECT_LIB} PRIVATE "${AREG_TESTS_DIR}")
target_compile_definitions(${AREG_FAULT_INJECT_LIB} PRIVATE AREG_FAULT_EXPORTS)
# Links with the target options of the compile, for example -m32.
target_link_libraries(${AREG_FAULT_INJECT_LIB} PRIVATE ${AREG_LDFLAGS})
if (WIN32)
    target_link_libraries(${AREG_FAULT_INJECT_LIB} PRIVATE ws2_32)
else()
    # The replaced open() must not meet the inline wrapper of the fortified C headers.
    target_compile_options(${AREG_FAULT_INJECT_LIB} PRIVATE -U_FORTIFY_SOURCE -fPIC)
    target_link_libraries(${AREG_FAULT_INJECT_LIB} PRIVATE ${CMAKE_DL_LIBS})
endif()
if (APPLE)
    # The thread sanitizer runtime calls the replaced open() while it initializes, so nothing here is instrumented.
    target_compile_options(${AREG_FAULT_INJECT_LIB} PRIVATE -fno-sanitize=thread)
endif()

addExecutableEx(${AREG_FAULT_INJECT_TEST} ""
    "${AREG_TESTS_DIR}/fault-inject/fault_inject_test.cpp;${AREG_TESTS_DIR}/fault-inject/fault_inject_router_client_test.cpp;${AREG_TESTS_DIR}/fault-inject/fault_inject_os_test.cpp;${AREG_TESTS_DIR}/fault-inject/fault_inject_alloc_sweep_test.cpp"
    "${AREG_FAULT_INJECT_LIB};GTest::gtest_main;GTest::gtest")

if (CYGWIN AND CMAKE_BUILD_TYPE MATCHES "Release")
    target_compile_options(${AREG_FAULT_INJECT_TEST} PRIVATE -O1)
endif()

if (APPLE OR CYGWIN OR ${AREG_ARCH} STREQUAL ${_proc_arm32} OR ${AREG_ARCH} STREQUAL ${_proc_arm64})
    gtest_discover_tests(${AREG_FAULT_INJECT_TEST} DISCOVERY_TIMEOUT 60 DISCOVERY_MODE PRE_TEST)
else()
    gtest_discover_tests(${AREG_FAULT_INJECT_TEST} DISCOVERY_TIMEOUT 60)
endif()
