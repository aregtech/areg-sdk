# ###########################################################################
# AREG SDK Examples with example specific functions
# Copyright 2022-2023 Aregtech
# ###########################################################################

# ###########################################################################
# List of examples to build
# ###########################################################################

if(AREG_BUILD_EXAMPLES)

    # Examples with specific design must be included one by one.
    include(${AREG_EXAMPLES}/00_helloservice/CMakeLists.txt)
    include(${AREG_EXAMPLES}/01_hello/CMakeLists.txt)
    include(${AREG_EXAMPLES}/02_buffer/CMakeLists.txt)
    include(${AREG_EXAMPLES}/03_file/CMakeLists.txt)
    include(${AREG_EXAMPLES}/04_logging/CMakeLists.txt)
    include(${AREG_EXAMPLES}/05_timer/CMakeLists.txt)
    include(${AREG_EXAMPLES}/06_threads/CMakeLists.txt)
    include(${AREG_EXAMPLES}/07_synch/CMakeLists.txt)
    include(${AREG_EXAMPLES}/08_service/CMakeLists.txt)
    include(${AREG_EXAMPLES}/09_svcmulti/CMakeLists.txt)
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
    include(${AREG_EXAMPLES}/22_pubsub/CMakeLists.txt)
    include(${AREG_EXAMPLES}/23_pubsubmix/CMakeLists.txt)
    include(${AREG_EXAMPLES}/24_pubsubmulti/CMakeLists.txt)

    if (AREG_DEVELOP_ENV MATCHES "Win32" AND NOT MINGW)
        # This projects are based either on MFC (Microsoft Foundation Classes)
        # or are compiled only under Windows. So that, they are available
        # only when compile with Microsoft Visual Studio.

        find_package(MFC)
        if (MFC_FOUND)
            # MFC specific projects. Can be compiled only if target machine has installed MFC.
            # Otherwise, output warning message and exclude from the build.
            include(${AREG_EXAMPLES}/17_winchat/CMakeLists.txt)
        else(MFC_FOUND)
            message(NOTICE " AREG Examples: >>> Due to missing installed MFC library, escaping compile projects:")
            message(NOTICE " AREG Examples: >>> ${AREG_EXAMPLES}/17_winchat")
        endif(MFC_FOUND)

    endif()

endif(AREG_BUILD_EXAMPLES)