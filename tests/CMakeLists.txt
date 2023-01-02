# ###########################################################################
# The google test settings

# ---------------------------------------------------------------------------
# Description : Compiles test source file and adds the specified test to be
#               discovered and run by CTest by passing the command line.
#               The command line of the test can be empty.
# Function ...: addTestEx
# usage ......: addTestEx( <test name> <source file> <test command line>) 
# ---------------------------------------------------------------------------
function(addTestEx testName testSource cmdLine)
    addExecutableEx(${testName} "${testSource}" "GTest::gtest_main")
    gtest_discover_tests(${testName})
endfunction(addTestEx testName testSource)

# ---------------------------------------------------------------------------
# Description : Compiles test source file equal to the test name. The path is
#               is relative to the 'units' folder and must not contain path
#               selparator symbol like '/'. It adds the test to be
#               discovered and run by CTest by passing the command line.
#               The command line of the test can be empty.
#               Use 'addTestEx' function if source file is not in 'units' folder.
# Function ...: addTestCmd
# usage ......: addTestCmd( <test name> <test command line>) 
# ---------------------------------------------------------------------------
function(addTestCmd testName cmdLine)
    addTestEx(${testName} "${AREG_UNIT_TEST_BASE}/${testName}" "${cmdLine}")
endfunction(addTestCmd testName cmdLine)

# ---------------------------------------------------------------------------
# Description : Compiles test source file equal to the test name. The path is
#               is relative to the 'units' folder and must not contain path
#               selparator symbol like '/'. It adds the test to be
#               discovered and run by CTest by using default command line.
#               Use 'addTestEx' function if source file is not in 'units' folder.
# Function ...: addTestCmd
# usage ......: addTestCmd( <test name> <test command line>) 
# ---------------------------------------------------------------------------
function(addTest testName)
    addTestEx(${testName} "${AREG_UNIT_TEST_BASE}/${testName}" "")
endfunction(addTest testName)

# Check whether the google test sources exist.
set(GOOGLE_TEST_BASE "${AREG_THIRDPARTY}/googletest")
if(EXISTS "${GOOGLE_TEST_BASE}/CMakeLists.txt")
    enable_testing()
    include_directories(${GOOGLE_TEST_BASE})
    include_directories(${AREG_TESTS})

    add_subdirectory(${GOOGLE_TEST_BASE})

    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    set(AREG_UNIT_TEST_BASE "${AREG_TESTS}/units")
    include(${AREG_UNIT_TEST_BASE}/CMakeLists.txt)
else()
    message(SEND_ERROR "Please clone the submodules using command\n
    '$ git clone --recurse-submodules https://github.com/aregtech/areg-sdk.git'")
endif()
