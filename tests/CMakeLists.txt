# ###########################################################################
# The google test settings

# ---------------------------------------------------------------------------
# Description : Compiles test source file and adds the specified unit test
#               to be discovered and run by CTest.
# Function ...: addTestEx
# usage ......: addTestEx( <test name> <source file>) 
# ---------------------------------------------------------------------------
function(addTestEx testName testSource)
    addExecutableEx(${testName} "${testSource}" "GTest::gtest_main")
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

# Check whether the google test sources exist.
set(GOOGLE_TEST_BASE "${AREG_THIRDPARTY}/googletest")
if(EXISTS "${GOOGLE_TEST_BASE}/CMakeLists.txt")

    set(AREG_UNIT_TEST_BASE "${AREG_TESTS}/units")
    enable_testing()
    include_directories(${GOOGLE_TEST_BASE})
    include_directories(${AREG_TESTS})

    add_subdirectory(${GOOGLE_TEST_BASE})

    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    include(${AREG_UNIT_TEST_BASE}/CMakeLists.txt)

else()

    message(SEND_ERROR "Please clone the submodules using command\n
    '$ git clone --recurse-submodules https://github.com/aregtech/areg-sdk.git'")

endif()
