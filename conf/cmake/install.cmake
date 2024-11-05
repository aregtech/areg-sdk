# ###########################################################################
# Configuration file to install AREG SDK modules.
# Copyright 2022-2023 Aregtech
# ###########################################################################

message(STATUS "AREG: >>> Preparing AREG SDK installation settings, Package name = '${CMAKE_PACKAGE_NAME}', Destination = '${CMAKE_INSTALL_PREFIX}'")

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
set(AREG_EXPORTS_DIR "${AREG_CMAKE_CONFIG_DIR}/../exports")
set(AREG_INSTALL_DST "${CMAKE_INSTALL_PREFIX}")

# Setup AREG SDK exporting targets.
target_include_directories(areg         PUBLIC $<INSTALL_INTERFACE:include>)
target_include_directories(aregextend   PUBLIC $<INSTALL_INTERFACE:include>)
target_include_directories(areglogger   PUBLIC $<INSTALL_INTERFACE:include>)
target_include_directories(logger       PUBLIC $<INSTALL_INTERFACE:include>)
target_include_directories(logobserver  PUBLIC $<INSTALL_INTERFACE:include>)
target_include_directories(mcrouter     PUBLIC $<INSTALL_INTERFACE:include>)

target_link_directories(areg            PUBLIC $<INSTALL_INTERFACE:lib> $<INSTALL_INTERFACE:bin>)
target_link_directories(aregextend      PUBLIC $<INSTALL_INTERFACE:lib> $<INSTALL_INTERFACE:bin>)
target_link_directories(areglogger      PUBLIC $<INSTALL_INTERFACE:lib> $<INSTALL_INTERFACE:bin>)
target_link_directories(logger          PUBLIC $<INSTALL_INTERFACE:lib> $<INSTALL_INTERFACE:bin>)
target_link_directories(logobserver     PUBLIC $<INSTALL_INTERFACE:lib> $<INSTALL_INTERFACE:bin>)
target_link_directories(mcrouter        PUBLIC $<INSTALL_INTERFACE:lib> $<INSTALL_INTERFACE:bin>)

# Copy all header files of AREG Framework
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

# Copy compiled binaries in the bin and lib directories
install(TARGETS areg aregextend areglogger
    EXPORT ${AREG_PACKAGE_NAME}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}  COMPONENT Development  COMPONENT Runtime
            PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ WORLD_READ GROUP_EXECUTE WORLD_EXECUTE
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}  COMPONENT Development  COMPONENT Runtime
            PERMISSIONS OWNER_READ OWNER_WRITE               GROUP_READ WORLD_READ
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}  COMPONENT Development  COMPONENT Runtime
            PERMISSIONS OWNER_READ OWNER_WRITE               GROUP_READ WORLD_READ
)

# Copy AREG configuration file
install(FILES "${AREG_FRAMEWORK}/areg/resources/areg.init"
            DESTINATION share/${AREG_PACKAGE_NAME}
            COMPONENT Development   COMPONENT Runtime
            PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ GROUP_WRITE WORLD_READ
            CONFIGURATIONS Release
)

# Copy AREG SDK open source license
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

# Copy 'areg.cmake' configuration file.
install(FILES ${AREG_SDK_ROOT}/areg.cmake
            DESTINATION share/${AREG_PACKAGE_NAME}/
            COMPONENT Development   COMPONENT Runtime
            PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
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

# Copy additionally areg and areglogger dynamic libraries
install(TARGETS areg areglogger
    LIBRARY DESTINATION tools/${AREG_PACKAGE_NAME}
            COMPONENT Development   COMPONENT Runtime
            CONFIGURATIONS Release
)

# Copy AREG configuration file
install(FILES "${AREG_FRAMEWORK}/areg/resources/areg.init"
            DESTINATION tools/${AREG_PACKAGE_NAME}/config
            COMPONENT Development   COMPONENT Runtime
            PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ GROUP_WRITE WORLD_READ
            CONFIGURATIONS Release
)

# Configure and install `pkg_config` files
configure_file("${AREG_EXPORTS_DIR}/areg.pc.in"         exports/areg.pc         @ONLY)
configure_file("${AREG_EXPORTS_DIR}/aregextend.pc.in"   exports/aregextend.pc   @ONLY)
configure_file("${AREG_EXPORTS_DIR}/areglogger.pc.in"   exports/areglogger.pc   @ONLY)

install(FILES 
            "${CMAKE_CURRENT_BINARY_DIR}/exports/areg.pc"
            "${CMAKE_CURRENT_BINARY_DIR}/exports/aregextend.pc"
            "${CMAKE_CURRENT_BINARY_DIR}/exports/areglogger.pc"
            DESTINATION lib/pkgconfig
            COMPONENT Development   COMPONENT Runtime
)

# configure and copy servicing script files.
if ((WIN32) OR (CYGWIN))

    configure_file("${AREG_EXPORTS_DIR}/logger.service.install.bat.in"      exports/logger.service.install.bat      @ONLY)
    configure_file("${AREG_EXPORTS_DIR}/logger.service.uninstall.bat.in"    exports/logger.service.uninstall.bat    @ONLY)
    configure_file("${AREG_EXPORTS_DIR}/mcrouter.service.install.bat.in"    exports/mcrouter.service.install.bat    @ONLY)
    configure_file("${AREG_EXPORTS_DIR}/mcrouter.service.uninstall.bat.in"  exports/mcrouter.service.uninstall.bat  @ONLY)
    install(FILES 
            "${CMAKE_CURRENT_BINARY_DIR}/exports/logger.service.install.bat"
            "${CMAKE_CURRENT_BINARY_DIR}/exports/logger.service.uninstall.bat"
            "${CMAKE_CURRENT_BINARY_DIR}/exports/mcrouter.service.install.bat"
            "${CMAKE_CURRENT_BINARY_DIR}/exports/mcrouter.service.uninstall.bat"
            DESTINATION share/${AREG_PACKAGE_NAME}/service
            COMPONENT Development   COMPONENT Runtime
            PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ GROUP_WRITE WORLD_READ
            CONFIGURATIONS Release
    )

else()

    configure_file("${AREG_EXPORTS_DIR}/logger.service.in"      "exports/logger.service"    @ONLY)
    configure_file("${AREG_EXPORTS_DIR}/mcrouter.service.in"    "exports/mcrouter.service"  @ONLY)
    install(FILES 
            "${CMAKE_CURRENT_BINARY_DIR}/exports/logger.service"
            "${CMAKE_CURRENT_BINARY_DIR}/exports/mcrouter.service"
            DESTINATION share/${AREG_PACKAGE_NAME}/service
            COMPONENT Development   COMPONENT Runtime
            PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ GROUP_WRITE WORLD_READ
            CONFIGURATIONS Release
    )

endif()

# Create and export 'cmake' configuration and version files.
write_basic_package_version_file(exports/${AREG_PACKAGE_NAME}-config-version.cmake VERSION ${AREG_PROJECT_VERSION} COMPATIBILITY AnyNewerVersion) 
configure_package_config_file("${AREG_EXPORTS_DIR}/config.cmake.in" 
                              "${CMAKE_CURRENT_BINARY_DIR}/exports/${AREG_PACKAGE_NAME}-config.cmake" 
                              INSTALL_DESTINATION share/${AREG_PACKAGE_NAME})

export(TARGETS areg aregextend areglogger
            NAMESPACE ${AREG_PACKAGE_NAME}::
            FILE ${AREG_PACKAGE_NAME}-targets.cmake
            EXPORT_LINK_INTERFACE_LIBRARIES
)

install(FILES
            "${CMAKE_CURRENT_BINARY_DIR}/exports/${AREG_PACKAGE_NAME}-config-version.cmake"
            "${CMAKE_CURRENT_BINARY_DIR}/exports/${AREG_PACKAGE_NAME}-config.cmake"
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

# Copy an example to create 'areg' based project.
install(DIRECTORY ${AREG_EXPORTS_DIR}/example
            DESTINATION share/${AREG_PACKAGE_NAME}
            COMPONENT Development   COMPONENT Runtime
            CONFIGURATIONS Release
)
