# ###########################################################################
# Areg SDK Examples with example specific functions
# Copyright 2022-2026 Aregtech
# ###########################################################################

# ###########################################################################
# List of examples to build
# ###########################################################################

if(AREG_EXAMPLES)

    # Examples with specific design must be included one by one.
    include(${AREG_EXAMPLES_DIR}/01_minimalrpc/CMakeLists.txt)
    include(${AREG_EXAMPLES_DIR}/02_minimalipc/CMakeLists.txt)
    include(${AREG_EXAMPLES_DIR}/03_helloservice/CMakeLists.txt)
    include(${AREG_EXAMPLES_DIR}/04_hellothread/CMakeLists.txt)
    include(${AREG_EXAMPLES_DIR}/05_buffer/CMakeLists.txt)
    include(${AREG_EXAMPLES_DIR}/06_file/CMakeLists.txt)
    include(${AREG_EXAMPLES_DIR}/07_logging/CMakeLists.txt)
    include(${AREG_EXAMPLES_DIR}/08_timer/CMakeLists.txt)
    include(${AREG_EXAMPLES_DIR}/09_threads/CMakeLists.txt)
    include(${AREG_EXAMPLES_DIR}/10_sync/CMakeLists.txt)
    include(${AREG_EXAMPLES_DIR}/11_service/CMakeLists.txt)
    include(${AREG_EXAMPLES_DIR}/12_svcmulti/CMakeLists.txt)
    include(${AREG_EXAMPLES_DIR}/13_locsvc/CMakeLists.txt)
    include(${AREG_EXAMPLES_DIR}/14_locmesh/CMakeLists.txt)
    include(${AREG_EXAMPLES_DIR}/15_pubsvc/CMakeLists.txt)
    include(${AREG_EXAMPLES_DIR}/16_pubmesh/CMakeLists.txt)
    include(${AREG_EXAMPLES_DIR}/17_pubtraffic/CMakeLists.txt)
    include(${AREG_EXAMPLES_DIR}/18_pubworker/CMakeLists.txt)
    include(${AREG_EXAMPLES_DIR}/19_pubfsm/CMakeLists.txt)
    include(${AREG_EXAMPLES_DIR}/21_locwatchdog/CMakeLists.txt)
    include(${AREG_EXAMPLES_DIR}/22_pubwatchdog/CMakeLists.txt)
    include(${AREG_EXAMPLES_DIR}/23_pubdatarate/CMakeLists.txt)
    include(${AREG_EXAMPLES_DIR}/24_pubunblock/CMakeLists.txt)
    include(${AREG_EXAMPLES_DIR}/25_pubsub/CMakeLists.txt)
    include(${AREG_EXAMPLES_DIR}/26_pubsubmix/CMakeLists.txt)
    include(${AREG_EXAMPLES_DIR}/27_pubsubmulti/CMakeLists.txt)
    include(${AREG_EXAMPLES_DIR}/28_stlsync/CMakeLists.txt)
    include(${AREG_EXAMPLES_DIR}/29_syncevent/CMakeLists.txt)
    include(${AREG_EXAMPLES_DIR}/30_publatency/CMakeLists.txt)

    if (AREG_DEVELOP_ENV MATCHES "Win32")
        # This projects are based either on MFC (Microsoft Foundation Classes)
        # or are compiled only under Windows. So that, they are available
        # only when compile with Microsoft Visual Studio.

        find_package(MFC)
        if (MFC_FOUND)
            # MFC specific projects. Can be compiled only if target machine has installed MFC.
            # Otherwise, output warning message and exclude from the build.
            include(${AREG_EXAMPLES_DIR}/20_winchat/CMakeLists.txt)
        else(MFC_FOUND)
            message(NOTICE " Areg Examples: >>> Due to missing installed MFC library, escaping compile projects:")
            message(NOTICE " Areg Examples: >>> ${AREG_EXAMPLES_DIR}/20_winchat")
        endif(MFC_FOUND)

    endif()

endif(AREG_EXAMPLES)