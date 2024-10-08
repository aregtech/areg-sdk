# ###########################################################################
# The Google test settings

if (AREG_BUILD_TESTS)

    # local variables
    set(AREG_GTEST_INCLUDES)
    set(AREG_GTEST_LIBRARY)

    enable_testing()

    if (AREG_GTEST_PACKAGE)
        # Check whether the Google Test sources exist.
        macro_find_package(GTest AREG_GTEST_FOUND AREG_GTEST_INCLUDES AREG_GTEST_LIBRARY)
    endif(AREG_GTEST_PACKAGE)

    if (AREG_GTEST_FOUND)
        # Google Test package found, make no actions on it, the libraries can be used.
        option(AREG_GTEST_FOUND  "GTest package found flag"   TRUE)
        message(STATUS "AREG: >>> Will use existing googletest package")
        if (NOT "${AREG_GTEST_INCLUDES}" STREQUAL "")
            include_directories("${AREG_GTEST_INCLUDES}")
        endif()

    else(AREG_GTEST_FOUND)

        option(AREG_GTEST_FOUND  "GTest package found flag"   FALSE)
        message(STATUS "AREG: >>> Will compile googletest libraries")
        # Google Test package did not find, fetch sources from repository.
        set(AREG_GTEST_PACKAGE OFF CACHE BOOL "Use GTest installed package" FORCE)
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
