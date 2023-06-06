# Examples which has similar building structure
list(APPEND projectList
    01_hello
    02_buffer
    03_file
    04_trace
    05_timer
    06_threads
    07_synch
    08_service
    09_svcmulti
)

# Examples with specific design must be included one by one.
include(${AREG_EXAMPLES}/00_helloservice/CMakeLists.txt)
include(${AREG_EXAMPLES}/10_locsvc/CMakeLists.txt)
include(${AREG_EXAMPLES}/11_locmesh/CMakeLists.txt)
include(${AREG_EXAMPLES}/12_pubsvc/CMakeLists.txt)
include(${AREG_EXAMPLES}/13_pubmesh/CMakeLists.txt)
include(${AREG_EXAMPLES}/14_pubtraffic/CMakeLists.txt)
include(${AREG_EXAMPLES}/15_pubworker/CMakeLists.txt)
include(${AREG_EXAMPLES}/16_pubfsm/CMakeLists.txt)
include(${AREG_EXAMPLES}/18_locwatchdog/CMakeLists.txt)
include(${AREG_EXAMPLES}/19_pubwatchdog/CMakeLists.txt)
include(${AREG_EXAMPLES}/20_pubdatarate/CMakeLists.txt)
include(${AREG_EXAMPLES}/21_pubunblock/CMakeLists.txt)

if (AREG_DEVELOP_ENV MATCHES "Win32")
    # This projects are based either on MFC (Microsoft Foundation Classes)
    # or are compiled only under Windows. So that, they are available
    # only when compile with Microsoft Visual Studio.

    find_package(MFC)
    if (MFC_FOUND)
        # MFC specific projects. Can be compiled only if target machine has installed MFC.
        # Otherwise, output warning message and exclude from the build.
        include(${AREG_EXAMPLES}/17_winchat/CMakeLists.txt)
    else(MFC_FOUND)
        message(NOTICE " >>> NOTE: due to missing installed MFC library, escaping compile projects:")
        message(NOTICE " >>>		${AREG_EXAMPLES}/17_winchat")
    endif(MFC_FOUND)

endif()

# Iterating over projectList and extract each source file,
# to create separate target and executable.
# Iterating over projectList and extract each source file,
# to create separate target and executable.
foreach(item IN LISTS projectList)
    unset(SRC)
    INCLUDE_DIR("${item}")

    # Adding target for each example.
    addExecutable(${item} "${SRC}")
    target_include_directories(${item} BEFORE PRIVATE "${AREG_EXAMPLES}/${item}")

endforeach()
