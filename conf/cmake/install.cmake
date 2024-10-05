# ###########################################################################
# Configuration file to install AREG SDK modules.
# Copyright 2022-2023 Aregtech
# ###########################################################################

message(STATUS "AREG: >>> Preparing AREG SDK installation settings, CMAKE_PACKAGE_NAME = ${CMAKE_PACKAGE_NAME}")

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
set(AREG_PACKAGE_NAME   "areg")
set(AREG_EXPORTS_DIR "${AREG_CMAKE_CONFIG_DIR}/../exports")
set(AREG_INSTALL_DST "${CMAKE_INSTALL_PREFIX}")

target_include_directories(areg PUBLIC $<INSTALL_INTERFACE:include>)

# Copy AREG SDK all headers
install(DIRECTORY framework/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} COMPONENT Development
    CONFIGURATIONS Release
    FILES_MATCHING
        PATTERN "*.h" 
        PATTERN "*.hpp"
        PATTERN "logger"            EXCLUDE
        PATTERN "logobserver"       EXCLUDE
        PATTERN "mcrouter"          EXCLUDE
)

# copy compiled binaries in the bin and lib directories
install(TARGETS areg aregextend areglogger
    EXPORT ${AREG_PACKAGE_NAME}
    RUNTIME DESTINATION bin  COMPONENT Development  COMPONENT Runtime
            PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ WORLD_READ GROUP_EXECUTE WORLD_EXECUTE
    LIBRARY DESTINATION bin  COMPONENT Development  COMPONENT Runtime
            PERMISSIONS OWNER_READ OWNER_WRITE               GROUP_READ WORLD_READ
    ARCHIVE DESTINATION lib  COMPONENT Development  COMPONENT Runtime
            PERMISSIONS OWNER_READ OWNER_WRITE               GROUP_READ WORLD_READ
)

# Copy AREG configuration file
install(DIRECTORY "${AREG_OUTPUT_BIN}/config"
            DESTINATION bin
            COMPONENT Development   COMPONENT Runtime
)

# Copy AREG SDK license
install(FILES LICENSE.txt
            DESTINATION share/${AREG_PACKAGE_NAME}
            COMPONENT Development   COMPONENT Runtime
            RENAME copyright
            PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
            CONFIGURATIONS Release
)

# Copy all CMake and MSVC configuration files.
install(DIRECTORY ${AREG_SDK_ROOT}/conf/
            DESTINATION share/${AREG_PACKAGE_NAME}/conf
            COMPONENT Development   COMPONENT Runtime
            CONFIGURATIONS Release
)

# Copy all tools
install(DIRECTORY tools/
            DESTINATION tools/${AREG_PACKAGE_NAME}
            COMPONENT Development
            DIRECTORY_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ WORLD_READ GROUP_EXECUTE WORLD_EXECUTE
            CONFIGURATIONS Release
)

# Copy compiled AREG SDK tools: logger, logobserver and mcrouter
install(TARGETS logger logobserver mcrouter
    EXPORT ${AREG_PACKAGE_NAME}
    RUNTIME DESTINATION tools/${AREG_PACKAGE_NAME}
            COMPONENT Development   COMPONENT Runtime
            PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ WORLD_READ GROUP_EXECUTE WORLD_EXECUTE
            CONFIGURATIONS Release
)

# Copy additionally areg library
install(TARGETS areg
    RUNTIME DESTINATION tools/${AREG_PACKAGE_NAME}
            COMPONENT Development   COMPONENT Runtime
            PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ WORLD_READ GROUP_EXECUTE WORLD_EXECUTE
            CONFIGURATIONS Release
)

# Copy AREG configuration file
install(DIRECTORY "${AREG_OUTPUT_BIN}/config"
            DESTINATION tools/${AREG_PACKAGE_NAME}
            COMPONENT Development   COMPONENT Runtime
            CONFIGURATIONS Release
)


configure_file("${AREG_EXPORTS_DIR}/areg.pc.in"         areg.pc         @ONLY)
configure_file("${AREG_EXPORTS_DIR}/aregextend.pc.in"   aregextend.pc   @ONLY)
configure_file("${AREG_EXPORTS_DIR}/areglogger.pc.in"   areglogger.pc   @ONLY)

install(FILES 
            "${CMAKE_CURRENT_BINARY_DIR}/areg.pc"
            "${CMAKE_CURRENT_BINARY_DIR}/aregextend.pc"
            "${CMAKE_CURRENT_BINARY_DIR}/areglogger.pc"
            DESTINATION lib/pkgconfig
            COMPONENT Development   COMPONENT Runtime
)

if ((WIN32) OR (CYGWIN))

    configure_file("${AREG_EXPORTS_DIR}/logger.service.install.bat.in"      "logger.service.install.bat"    @ONLY)
    configure_file("${AREG_EXPORTS_DIR}/logger.service.uninstall.bat.in"    "logger.service.uninstall.bat"  @ONLY)
    configure_file("${AREG_EXPORTS_DIR}/mcrouter.service.install.bat.in"    "mcrouter.service.install.bat"  @ONLY)
    configure_file("${AREG_EXPORTS_DIR}/mcrouter.service.uninstall.bat.in"  "mcrouter.service.uninstall.bat" @ONLY)
    install(FILES 
            "${CMAKE_CURRENT_BINARY_DIR}/logger.service.install.bat"
            "${CMAKE_CURRENT_BINARY_DIR}/logger.service.uninstall.bat"
            "${CMAKE_CURRENT_BINARY_DIR}/mcrouter.service.install.bat"
            "${CMAKE_CURRENT_BINARY_DIR}/mcrouter.service.uninstall.bat"
            DESTINATION tools/${AREG_PACKAGE_NAME}
            COMPONENT Development   COMPONENT Runtime
            CONFIGURATIONS Release
    )

else()

    configure_file("${AREG_EXPORTS_DIR}/logger.service.in"      "logger.service"    @ONLY)
    configure_file("${AREG_EXPORTS_DIR}/mcrouter.service.in"    "mcrouter.service"  @ONLY)
    install(FILES 
            "${CMAKE_CURRENT_BINARY_DIR}/logger.service"
            "${CMAKE_CURRENT_BINARY_DIR}/mcrouter.service"
            DESTINATION share/${AREG_PACKAGE_NAME}/service
            COMPONENT Development   COMPONENT Runtime
            CONFIGURATIONS Release
    )

endif()

write_basic_package_version_file(${AREG_PACKAGE_NAME}-config-version.cmake VERSION ${AREG_PROJECT_VERSION} COMPATIBILITY AnyNewerVersion) 
configure_package_config_file("${AREG_EXPORTS_DIR}/config.cmake.in" "${CMAKE_CURRENT_BINARY_DIR}/${AREG_PACKAGE_NAME}-config.cmake" INSTALL_DESTINATION share/${AREG_PACKAGE_NAME})

export(TARGETS areg aregextend areglogger
            NAMESPACE ${AREG_PACKAGE_NAME}::
            FILE ${AREG_PACKAGE_NAME}-targets.cmake
            EXPORT_LINK_INTERFACE_LIBRARIES
)

install(FILES
            "${CMAKE_CURRENT_BINARY_DIR}/${AREG_PACKAGE_NAME}-config-version.cmake"
            "${CMAKE_CURRENT_BINARY_DIR}/${AREG_PACKAGE_NAME}-config.cmake"
            DESTINATION share/${AREG_PACKAGE_NAME}
            COMPONENT Development
)

install(EXPORT ${AREG_PACKAGE_NAME} 
            DESTINATION share/${AREG_PACKAGE_NAME} 
            COMPONENT Development
            NAMESPACE ${AREG_PACKAGE_NAME}::
            FILE ${AREG_PACKAGE_NAME}-targets.cmake
            EXPORT_LINK_INTERFACE_LIBRARIES
)
