# ###########################################################################
# Areg SDK Examples with example specific functions
# Copyright 2022-2026 Aregtech
# ###########################################################################

# ###########################################################################
# List of examples to build
# ###########################################################################

if(AREG_BUILD_EXAMPLES)

    # Examples with specific design must be included one by one.
    include(${AREG_EXAMPLES}/01_minimalrpc/CMakeLists.txt)
    include(${AREG_EXAMPLES}/02_minimalipc/CMakeLists.txt)
    include(${AREG_EXAMPLES}/03_helloservice/CMakeLists.txt)
    include(${AREG_EXAMPLES}/04_hellothread/CMakeLists.txt)
    include(${AREG_EXAMPLES}/05_buffer/CMakeLists.txt)
    include(${AREG_EXAMPLES}/06_file/CMakeLists.txt)
    include(${AREG_EXAMPLES}/07_logging/CMakeLists.txt)
    include(${AREG_EXAMPLES}/08_timer/CMakeLists.txt)
    include(${AREG_EXAMPLES}/09_threads/CMakeLists.txt)
    include(${AREG_EXAMPLES}/10_sync/CMakeLists.txt)
    include(${AREG_EXAMPLES}/11_service/CMakeLists.txt)
    include(${AREG_EXAMPLES}/12_svcmulti/CMakeLists.txt)
    include(${AREG_EXAMPLES}/13_locsvc/CMakeLists.txt)
    include(${AREG_EXAMPLES}/14_locmesh/CMakeLists.txt)
    include(${AREG_EXAMPLES}/15_pubsvc/CMakeLists.txt)
    include(${AREG_EXAMPLES}/16_pubmesh/CMakeLists.txt)
    include(${AREG_EXAMPLES}/17_pubtraffic/CMakeLists.txt)
    include(${AREG_EXAMPLES}/18_pubworker/CMakeLists.txt)
    include(${AREG_EXAMPLES}/19_pubfsm/CMakeLists.txt)
    include(${AREG_EXAMPLES}/21_locwatchdog/CMakeLists.txt)
    include(${AREG_EXAMPLES}/22_pubwatchdog/CMakeLists.txt)
    include(${AREG_EXAMPLES}/23_pubdatarate/CMakeLists.txt)
    include(${AREG_EXAMPLES}/24_pubunblock/CMakeLists.txt)
    include(${AREG_EXAMPLES}/25_pubsub/CMakeLists.txt)
    include(${AREG_EXAMPLES}/26_pubsubmix/CMakeLists.txt)
    include(${AREG_EXAMPLES}/27_pubsubmulti/CMakeLists.txt)
    include(${AREG_EXAMPLES}/28_stlsync/CMakeLists.txt)
    include(${AREG_EXAMPLES}/29_syncevent/CMakeLists.txt)

    if (AREG_DEVELOP_ENV MATCHES "Win32")
        # This projects are based either on MFC (Microsoft Foundation Classes)
        # or are compiled only under Windows. So that, they are available
        # only when compile with Microsoft Visual Studio.

        find_package(MFC)
        if (MFC_FOUND)
            # MFC specific projects. Can be compiled only if target machine has installed MFC.
            # Otherwise, output warning message and exclude from the build.
            include(${AREG_EXAMPLES}/20_winchat/CMakeLists.txt)
        else(MFC_FOUND)
            message(NOTICE " Areg Examples: >>> Due to missing installed MFC library, escaping compile projects:")
            message(NOTICE " Areg Examples: >>> ${AREG_EXAMPLES}/20_winchat")
        endif(MFC_FOUND)

    endif()

endif(AREG_BUILD_EXAMPLES)