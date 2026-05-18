
# ###########################################################################
# Areg Unit tests
# Copyright 2022-2026 Aregtech
# ###########################################################################

# ---------------------------------------------------------------------------
# Description : Creates an unit test executable from specified list of sources
#               linked with 'areg' and 'gtest' libraries. If the target with the
#               same name exist, the sources are added to the existing executable.
#               Otherwise, the new executable is created using specified source.
# macro ......: macro_add_unit_test
# Parameters .: ${test_project}     -- The name unit test executable.
# Usage ......: macro_add_unit_test( <name of executable> <list of sources>)
# ---------------------------------------------------------------------------
macro(macro_add_unit_test test_project)

    set(_tests)
    set(_list "${ARGN}")
    set(_libs)

    foreach(item IN LISTS _list)
        list(APPEND _tests "${AREG_UNIT_TEST_BASE}/${item}")
    endforeach()

    list(APPEND _libs "GTest::gtest_main" "GTest::gtest")
    addExecutableEx(${test_project} "" "${_tests}" "${_libs}")

    # Fix of Cygwin unit test compilation.
    if (CYGWIN AND CMAKE_BUILD_TYPE MATCHES "Release")
        target_compile_options(${test_project} PRIVATE -O1)
    endif()

    # Fix of Cygwin unit test compilation.
    if (CYGWIN OR ${AREG_ARCH} STREQUAL ${_proc_arm32} OR ${AREG_ARCH} STREQUAL ${_proc_arm64})
        gtest_discover_tests(${test_project} DISCOVERY_TIMEOUT 60 DISCOVERY_MODE PRE_TEST)
    else()
        gtest_discover_tests(${test_project} DISCOVERY_TIMEOUT 60)
    endif()

    unset(_tests)
    unset(_list)
    unset(_libs)

endmacro(macro_add_unit_test)

macro_add_unit_test("${AREG_UNIT_TEST_PROJECT}"
    GUnitTest.cpp
    DateTimeTest.cpp
    FileTest.cpp
    LogScopesTest.cpp
    OptionParserTest.cpp
    StringDefsTest.cpp
    StringDefsTest2.cpp
    StringDefsTest3.cpp
    StringUtilsTest.cpp
    ArrayListTest.cpp
    FixedArrayTest.cpp
    HashMapTest.cpp
    LinkedListTest.cpp
    MapTest.cpp
    KeyValuePairTest.cpp
    RingStackTest.cpp
    SharedBufferTest.cpp
    RemoteMessageTest.cpp
    SortedLinkedListTest.cpp
    StackTest.cpp
)
