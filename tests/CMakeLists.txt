# ###########################################################################
# The Google test settings

if (AREG_BUILD_TESTS)

    # Check whether the Google test sources exist.
    include(FetchContent)
    enable_testing()
    find_package(GTest)
    if (NOT GTest_FOUND)
        message(STATUS "Fetches googletest from https://github.com/google/googletest.git repo to ${AREG_PACKAGES} directory.")
        # Fetch the latest sources from googletest repository.
        FetchContent_Declare( googletest
                              GIT_REPOSITORY https://github.com/google/googletest.git
                              GIT_TAG        "main")
    endif()

	FetchContent_MakeAvailable(googletest)
    set(GOOGLE_TEST_BASE "${GTEST_INCLUDE_DIRS}")
    include_directories(${GOOGLE_TEST_BASE})
    add_library(GTest::GTest INTERFACE IMPORTED)
    target_link_libraries(GTest::GTest INTERFACE gtest_main)

    # set areg-sdk project settings.
    set(AREG_UNIT_TEST_BASE "${AREG_TESTS}/units")
    set(AREG_UNIT_TEST_PROJECT "areg-unit-tests")
    include_directories(${AREG_TESTS})

    # Include AREG SDK unit test directory.
    include(GoogleTest)

    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    include(${AREG_UNIT_TEST_BASE}/CMakeLists.txt)

endif()
