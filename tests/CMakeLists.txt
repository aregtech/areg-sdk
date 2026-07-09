# ###########################################################################
# The Google test settings
# Copyright 2022-2026 Aregtech (Artak Avetyan)
# ###########################################################################

# local variables
set(AREG_GTEST_INCLUDES)
set(AREG_GTEST_LIBRARY)
option(AREG_GTEST_FOUND  "GTest package found flag"   FALSE)

if (AREG_TESTS)

    # Disable unit tests post-build for ARM processor, Cygwin, and macOS.
    # On Cygwin the binary may crash at startup in Release due to PE/COFF + visibility
    # interactions. On macOS with system GTest (AREG_SYSTEM_GTEST=ON), the test binary
    # references a Homebrew dylib via @rpath; running it immediately after linking fails
    # because the Homebrew lib dir is not in the default RPATH. PRE_TEST defers discovery
    # to ctest so the build is not blocked.
    if (${AREG_ARCH} STREQUAL ${_proc_arm32} OR ${AREG_ARCH} STREQUAL ${_proc_arm64} OR CYGWIN OR APPLE)
        set(CMAKE_GTEST_DISCOVER_TESTS_DISCOVERY_MODE PRE_TEST)
    endif()

    enable_testing()

    if (AREG_SYSTEM_GTEST)
        # Check whether the Google Test sources exist.
        macro_find_gtest_package(AREG_GTEST_INCLUDES AREG_GTEST_LIBRARY AREG_GTEST_FOUND)
    endif()

    if (AREG_GTEST_FOUND)
        # Google Test package found, make no actions on it, the libraries can be used.
        message(STATUS "Areg: >>> Will use existing in the system googletest package")
        if (NOT "${AREG_GTEST_INCLUDES}" STREQUAL "")
            include_directories("${AREG_GTEST_INCLUDES}")
        endif()
        # On macOS, system GTest (e.g. Homebrew) is a shared library.  The test binary
        # references it via @rpath.  Without the Homebrew lib directory in CMAKE_BUILD_RPATH
        # the dyld loader cannot find the dylib at ctest time.  Derive the directory from
        # the first library in AREG_GTEST_LIBRARY and append it so every test target built
        # in this directory picks it up automatically.
        if (APPLE AND NOT "${AREG_GTEST_LIBRARY}" STREQUAL "")
            set(_gtest_lib_path)
            list(GET AREG_GTEST_LIBRARY 0 _gtest_lib_path)
            if (NOT EXISTS "${_gtest_lib_path}")
                get_target_property(_gtest_lib_path GTest::gtest IMPORTED_LOCATION)
            endif()
            if (NOT EXISTS "${_gtest_lib_path}")
                get_target_property(_gtest_lib_path GTest::GTest IMPORTED_LOCATION)
            endif()
            if (_gtest_lib_path AND EXISTS "${_gtest_lib_path}")
                get_filename_component(_gtest_dir "${_gtest_lib_path}" DIRECTORY)
                list(APPEND CMAKE_BUILD_RPATH "${_gtest_dir}")
                message(STATUS "Areg: >>> Added GTest directory to CMAKE_BUILD_RPATH: '${_gtest_dir}'")
            endif()
            unset(_gtest_lib_path)
            unset(_gtest_dir)
        endif()
    else()
        message(STATUS "Areg: >>> Fetching \'googletest\' sources to compile \'gtest\' libraries")
        # Google Test package did not find, fetch sources from repository.
        set(AREG_SYSTEM_GTEST OFF)
        include(FetchContent)
        # Fetch the latest sources from googletest repository.
        # Pin to a stable release tag. Using "main" pulls in-development code that
        # may use C++20 attributes (e.g. [[nodiscard]] on abstract base classes) in
        # C++17 mode, causing GCC to generate subtly wrong code for template-heavy
        # parameterized test registration, which crashes on Cygwin Release builds.
        FetchContent_Declare( googletest
                              GIT_REPOSITORY https://github.com/google/googletest.git
                              GIT_TAG        "v1.15.2")

        # For Windows: Prevent overriding the parent project's compiler/linker settings
        set(gtest_force_shared_crt ON CACHE BOOL "For Windows: Prevent overriding the parent project\'s compiler/linker settings" FORCE)

        # Always build GTest as a static library.
        set(_areg_saved_build_shared_libs ${BUILD_SHARED_LIBS})
        set(BUILD_SHARED_LIBS OFF)
	    FetchContent_MakeAvailable(googletest)
        set(BUILD_SHARED_LIBS ${_areg_saved_build_shared_libs})
        unset(_areg_saved_build_shared_libs)

        if (NOT "${GTEST_INCLUDE_DIRS}" STREQUAL "")
            include_directories("${GTEST_INCLUDE_DIRS}")
        endif()

        include(GoogleTest)

        # Disable some warnings.
        target_compile_options(gtest PRIVATE "${AREG_OPT_DISABLE_WARN_THIRDPARTY}")
        target_compile_options(gtest_main PRIVATE "${AREG_OPT_DISABLE_WARN_THIRDPARTY}")

        target_compile_options(gmock PRIVATE "${AREG_OPT_DISABLE_WARN_THIRDPARTY}")
        target_compile_options(gmock_main PRIVATE "${AREG_OPT_DISABLE_WARN_THIRDPARTY}")

        # On Cygwin Release, add_compile_options() propagates -O3 to GTest targets.
        # GTest's template-heavy parameterized test registration code crashes under -O3
        # on Cygwin because the optimizer makes unsafe assumptions about temporary
        # lifetimes at PE/DLL call boundaries. Override with -O1 for GTest targets only.
        if (CYGWIN AND CMAKE_BUILD_TYPE MATCHES "Release")
            target_compile_options(gtest      PRIVATE -O1)
            target_compile_options(gtest_main PRIVATE -O1)
            target_compile_options(gmock      PRIVATE -O1)
            target_compile_options(gmock_main PRIVATE -O1)
        endif()

        # compile gtest from source like imported interface to use to create unit tests.
        add_library(GTest::GTest INTERFACE IMPORTED)
        target_link_libraries(GTest::GTest INTERFACE gtest_main)

    endif(AREG_GTEST_FOUND)

    # Include Areg SDK unit test directory.
    set(AREG_UNIT_TEST_BASE "${AREG_TESTS_DIR}/units")
    set(AREG_UNIT_TEST_PROJECT "areg-unit-tests")
    include_directories(${AREG_TESTS_DIR})
    include(${AREG_UNIT_TEST_BASE}/CMakeLists.txt)

else(AREG_TESTS)
    option(AREG_SYSTEM_GTEST  "Use GTest package" FALSE)
endif(AREG_TESTS)
