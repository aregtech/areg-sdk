
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
    gtest_discover_tests(${test_project} DISCOVERY_TIMEOUT 60)

    unset(_tests)
    unset(_list)
    unset(_libs)

endmacro(macro_add_unit_test)

macro_add_unit_test("${AREG_UNIT_TEST_PROJECT}"
    GUnitTest.cpp
    DateTimeTest.cpp
    FileTest.cpp
    LogScopesTest.cpp
    NEStringTest.cpp
    OptionParserTest.cpp
    StringUtilsTest.cpp
    TEArrayListTest.cpp
    TEFixedArrayTest.cpp
    TEHashMapTest.cpp
    TELinkedListTest.cpp
    TEMapTest.cpp
    TEPropertyTest.cpp
    TERingStackTest.cpp
    TESortedLinkedListTest.cpp
    TEStackTest.cpp
)
