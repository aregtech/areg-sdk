if (AREG_BUILD_TESTS)
# Check whether the Google test sources exist.
    find_package(GTest)
    if (GTEST_FOUND)

        message(STATUS ">>> Found installed package GTest, will use the installed package.")
        set(GOOGLE_TEST_BASE "${GTEST_INCLUDE_DIRS}")
        enable_testing()
        include_directories(${GOOGLE_TEST_BASE})

    elseif (EXISTS "${AREG_THIRDPARTY}/googletest/CMakeLists.txt")

        message(STATUS ">>> No GTest package found, will use the AREG SDK submodule sources.")
        set(GOOGLE_TEST_BASE "${AREG_THIRDPARTY}/googletest")
        enable_testing()
        include_directories(${GOOGLE_TEST_BASE})
        add_subdirectory("${GOOGLE_TEST_BASE}")

    else()

        message(SEND_ERROR "Please clone the submodules using command\n
        '$ git clone --recurse-submodules https://github.com/aregtech/areg-sdk.git'")

    endif()
endif(AREG_BUILD_TESTS)

include(${AREG_THIRDPARTY}/sqlite3/CMakeLists.txt)
