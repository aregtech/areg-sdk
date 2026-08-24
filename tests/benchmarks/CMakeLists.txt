
# ###########################################################################
# Areg Framework benchmarks
# Copyright 2022-2026 Aregtech (Artak Avetyan)
# ###########################################################################

# The benchmarks are compiled against the CodSpeed compatibility layer of the
# 'google_benchmark' library. The layer behaves exactly like the upstream
# 'google_benchmark' when 'CODSPEED_MODE' is not set, and reports the measured
# results to CodSpeed when it is set to 'simulation', 'walltime' or 'memory'.
#   $ cmake -B ./build -DAREG_BENCHMARKS=ON -DCODSPEED_MODE=simulation
#   $ cmake --build ./build --target areg-benchmarks -j

if (AREG_BENCHMARKS)

    set(AREG_BENCHMARK_PROJECT  "areg-benchmarks")
    set(AREG_BENCHMARK_BASE     "${AREG_BENCHMARKS_DIR}")

    message(STATUS "Areg: >>> Fetching CodSpeed \'google_benchmark\' sources to compile the benchmarks")

    include(FetchContent)

    # Do not compile anything of the 'google_benchmark' library except the library itself.
    set(BENCHMARK_ENABLE_TESTING        OFF CACHE BOOL "Disable google_benchmark tests"          FORCE)
    set(BENCHMARK_ENABLE_GTEST_TESTS    OFF CACHE BOOL "Disable google_benchmark gtest tests"    FORCE)
    set(BENCHMARK_ENABLE_INSTALL        OFF CACHE BOOL "Disable google_benchmark installation"   FORCE)
    set(BENCHMARK_INSTALL_DOCS          OFF CACHE BOOL "Disable google_benchmark documentation"  FORCE)
    set(BENCHMARK_ENABLE_WERROR         OFF CACHE BOOL "Disable google_benchmark warning errors" FORCE)

    FetchContent_Declare( google_benchmark
                          GIT_REPOSITORY https://github.com/CodSpeedHQ/codspeed-cpp.git
                          GIT_TAG        "v2.4.0"
                          SOURCE_SUBDIR  google_benchmark)

    # Always build the benchmark library as a static library.
    set(_areg_saved_build_shared_libs ${BUILD_SHARED_LIBS})
    set(BUILD_SHARED_LIBS OFF)
    FetchContent_MakeAvailable(google_benchmark)
    set(BUILD_SHARED_LIBS ${_areg_saved_build_shared_libs})
    unset(_areg_saved_build_shared_libs)

    # Disable the warnings of the third-party sources.
    if (TARGET benchmark)
        target_compile_options(benchmark PRIVATE "${AREG_OPT_DISABLE_WARN_THIRDPARTY}")
    endif()

    # The benchmark sources include the headers relative to the 'tests' directory.
    include_directories(${AREG_TESTS_DIR})

    set(_benchmarks)
    foreach(item IN ITEMS  BenchmarkMain.cpp
                           ContainerBenchmark.cpp
                           DateTimeBenchmark.cpp
                           SharedBufferBenchmark.cpp
                           StringBenchmark.cpp)
        list(APPEND _benchmarks "${AREG_BENCHMARK_BASE}/${item}")
    endforeach()

    addExecutableEx(${AREG_BENCHMARK_PROJECT} "" "${_benchmarks}" "benchmark::benchmark")
    target_include_directories(${AREG_BENCHMARK_PROJECT} BEFORE PRIVATE ${AREG_TESTS_DIR})

    # Set dependency of 'areg-dummy' to make sure that the output
    # directories exist before the benchmark executable is linked.
    if (TARGET areg-dummy)
        add_dependencies(${AREG_BENCHMARK_PROJECT} areg-dummy)
    endif()

    unset(_benchmarks)

endif(AREG_BENCHMARKS)
