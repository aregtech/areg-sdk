# ###########################################################################
# The Google test settings

# ---------------------------------------------------------------------------
# Description : Compiles test source file and adds the specified unit test
#               to be discovered and run by CTest.
# Function ...: addTestEx
# usage ......: addTestEx( <test name> <source file>) 
# ---------------------------------------------------------------------------
function(addTestEx testName testSource)
    list(APPEND google_test_libs "GTest::gtest_main" "GTest::gtest")
    addExecutableEx(${testName} "${testSource}" "${google_test_libs}")
    gtest_discover_tests(${testName} DISCOVERY_TIMEOUT 60)
endfunction(addTestEx testName testSource)

# ---------------------------------------------------------------------------
# Description : Compiles test source file, which is same as a test executable.
#               The source file location is in 'units' folder and must not
#               path separator symbol like '/'. The created executable has
#               same name with the extension of executable. This means that
#               if the source file is 'my_test.cpp', the generated executable
#               will have name 'my_test.cpp.exe' or 'my_test.cpp.out' for Linux.
#               Use 'addTestEx' function if source file is not in 'units' folder.
# Function ...: addTest
# usage ......: addTest( <test name> )
# ---------------------------------------------------------------------------
function(addTest testName)
    addTestEx(${testName} "${AREG_UNIT_TEST_BASE}/${testName}" "")
endfunction(addTest testName)

if (AREG_BUILD_TESTS)
# Check whether the Google test sources exist.
    include(FetchContent)
    enable_testing()
    find_package(GTest)
    if (NOT GTest_FOUND)
        message(STATUS "Fetching googletest from https://github.com/google/googletest.git to ${AREG_PACKAGES} directory.")
        FetchContent_Declare( googletest
                              GIT_REPOSITORY https://github.com/google/googletest.git
                              GIT_TAG        "main")
    endif()

	FetchContent_MakeAvailable(googletest)
    set(GOOGLE_TEST_BASE "${GTEST_INCLUDE_DIRS}")
    include_directories(${GOOGLE_TEST_BASE})
    add_library(GTest::GTest INTERFACE IMPORTED)
    target_link_libraries(GTest::GTest INTERFACE gtest_main)

    set(AREG_UNIT_TEST_BASE "${AREG_TESTS}/units")
    set(AREG_UNIT_TEST_PROJECT "areg-unit-tests")
    include_directories(${AREG_TESTS})

    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    include(${AREG_UNIT_TEST_BASE}/CMakeLists.txt)

endif()
