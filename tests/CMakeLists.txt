# ###########################################################################
# The Google test settings
# Copyright 2022-2023 Aregtech
# ###########################################################################

# local variables
set(AREG_GTEST_INCLUDES)
set(AREG_GTEST_LIBRARY)
option(AREG_GTEST_FOUND  "GTest package found flag"   FALSE)

if (AREG_BUILD_TESTS)

    # Disable unit tests post-build for ARM processor
    if (${AREG_PROCESSOR} STREQUAL ${_proc_arm32} OR ${AREG_PROCESSOR} STREQUAL ${_proc_arm64})
        set(CMAKE_GTEST_DISCOVER_TESTS_DISCOVERY_MODE PRE_TEST)
    endif()

    enable_testing()

    if (AREG_GTEST_PACKAGE)
        # Check whether the Google Test sources exist.
        macro_find_gtest_package("" AREG_GTEST_INCLUDES AREG_GTEST_LIBRARY AREG_GTEST_FOUND)
    endif()

    if (AREG_GTEST_FOUND)
        # Google Test package found, make no actions on it, the libraries can be used.
        message(STATUS "AREG: >>> Will use existing in the system googletest package")
        if (NOT "${AREG_GTEST_INCLUDES}" STREQUAL "")
            include_directories("${AREG_GTEST_INCLUDES}")
        endif()
    else()
        message(STATUS "AREG: >>> Fetching \'googletest\' sources to compile \'gtest\' libraries")
        # Google Test package did not find, fetch sources from repository.
        set(AREG_GTEST_PACKAGE OFF)
        include(FetchContent)
        # Fetch the latest sources from googletest repository.
        FetchContent_Declare( googletest
                              GIT_REPOSITORY https://github.com/google/googletest.git
                              GIT_TAG        "main")

        # For Windows: Prevent overriding the parent project's compiler/linker settings
        set(gtest_force_shared_crt ON CACHE BOOL "For Windows: Prevent overriding the parent project\'s compiler/linker settings" FORCE)
	    FetchContent_MakeAvailable(googletest)

        if (NOT "${GTEST_INCLUDE_DIRS}" STREQUAL "")
            include_directories("${GTEST_INCLUDE_DIRS}")
        endif()

        include(GoogleTest)

        # Disable some warnings.
        target_compile_options(gtest PRIVATE "${AREG_OPT_DISABLE_WARN_THIRDPARTY}")
        target_compile_options(gtest_main PRIVATE "${AREG_OPT_DISABLE_WARN_THIRDPARTY}")

        target_compile_options(gmock PRIVATE "${AREG_OPT_DISABLE_WARN_THIRDPARTY}")
        target_compile_options(gmock_main PRIVATE "${AREG_OPT_DISABLE_WARN_THIRDPARTY}")

        # compile gtest from source like imported interface to use to create unit tests.
        add_library(GTest::GTest INTERFACE IMPORTED)
        target_link_libraries(GTest::GTest INTERFACE gtest_main)

    endif(AREG_GTEST_FOUND)

    # Include AREG SDK unit test directory.
    set(AREG_UNIT_TEST_BASE "${AREG_TESTS}/units")
    set(AREG_UNIT_TEST_PROJECT "areg-unit-tests")
    include_directories(${AREG_TESTS})
    include(${AREG_UNIT_TEST_BASE}/CMakeLists.txt)

endif()
