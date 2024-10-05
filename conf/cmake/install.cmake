# ###########################################################################
# Configuration file to install AREG SDK modules.
# Copyright 2022-2023 Aregtech
# ###########################################################################

message(STATUS "AREG: >>> Preparing AREG SDK installation settings, CMAKE_PACKAGE_NAME = ${CMAKE_PACKAGE_NAME}")

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
set(AREG_PACKAGE_NAME   "areg")

target_include_directories(areg PUBLIC
	$<INSTALL_INTERFACE:include>
)

# copy compiled binaries in the bin and lib directories
install(TARGETS areg aregextend areglogger
    EXPORT ${AREG_PACKAGE_NAME}
    RUNTIME DESTINATION bin  COMPONENT Development  COMPONENT ${AREG_PACKAGE_NAME}
            PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ WORLD_READ GROUP_EXECUTE WORLD_EXECUTE
    LIBRARY DESTINATION bin  COMPONENT Development  COMPONENT ${AREG_PACKAGE_NAME}
            PERMISSIONS OWNER_READ OWNER_WRITE               GROUP_READ WORLD_READ
    ARCHIVE DESTINATION lib  COMPONENT Development  COMPONENT ${AREG_PACKAGE_NAME}
            PERMISSIONS OWNER_READ OWNER_WRITE               GROUP_READ WORLD_READ
)

install(TARGETS logger logobserver mcrouter
    EXPORT ${AREG_PACKAGE_NAME}
    RUNTIME DESTINATION tools/${AREG_PACKAGE_NAME}-dbg  COMPONENT Runtime      COMPONENT ${AREG_PACKAGE_NAME}
            PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ WORLD_READ GROUP_EXECUTE WORLD_EXECUTE
)

# Copy AREG SDK all headers
install(DIRECTORY framework/
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} COMPONENT Development   COMPONENT ${AREG_PACKAGE_NAME}
            CONFIGURATIONS Release
            FILES_MATCHING
                PATTERN "*.h" 
                PATTERN "*.hpp"
                PATTERN "logger"            EXCLUDE
                PATTERN "logobserver"       EXCLUDE
                PATTERN "mcrouter"          EXCLUDE
)

# Copy all CMake and MSVC configuration files.
install(DIRECTORY ${AREG_SDK_ROOT}/conf
            DESTINATION conf COMPONENT Development   COMPONENT ${AREG_PACKAGE_NAME}
)

# Copy all tools
install(DIRECTORY tools/
            DESTINATION tools/${AREG_PACKAGE_NAME}  COMPONENT Development   COMPONENT ${AREG_PACKAGE_NAME}
            DIRECTORY_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ WORLD_READ GROUP_EXECUTE WORLD_EXECUTE
)

# Copy AREG SDK license
install(FILES LICENSE.txt
            DESTINATION share/${AREG_PACKAGE_NAME} COMPONENT Development   COMPONENT Runtime   COMPONENT ${AREG_PACKAGE_NAME}
                RENAME copyright
                PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
)

# Copy AREG configuration file
install(DIRECTORY ${AREG_OUTPUT_BIN}/config
            DESTINATION bin             COMPONENT Runtime       COMPONENT ${AREG_PACKAGE_NAME}
)
    
# Copy 'logger' and 'mcrouter' service configuration files.
install(DIRECTORY ${AREG_OUTPUT_BIN}/
            DESTINATION bin/config      COMPONENT Runtime       COMPONENT ${AREG_PACKAGE_NAME}
            FILES_MATCHING
                PATTERN "mcrouter.service*"
                PATTERN "logger.service*"
                PATTERN "config" EXCLUDE
)

configure_file("${AREG_CMAKE_CONFIG_DIR}/areg.pc.in" areg.pc @ONLY)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/areg.pc" DESTINATION lib/pkgconfig)
configure_file("${AREG_CMAKE_CONFIG_DIR}/aregextend.pc.in" aregextend.pc @ONLY)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/aregextend.pc" DESTINATION lib/pkgconfig)
configure_file("${AREG_CMAKE_CONFIG_DIR}/areglogger.pc.in" areglogger.pc @ONLY)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/areglogger.pc" DESTINATION lib/pkgconfig)
install(EXPORT ${AREG_PACKAGE_NAME} DESTINATION share/${AREG_PACKAGE_NAME} NAMESPACE ${AREG_PACKAGE_NAME}:: FILE areg.cmake EXPORT_LINK_INTERFACE_LIBRARIES)
install(EXPORT ${AREG_PACKAGE_NAME} DESTINATION share/${AREG_PACKAGE_NAME} NAMESPACE ${AREG_PACKAGE_NAME}:: FILE aregextend.cmake EXPORT_LINK_INTERFACE_LIBRARIES)
install(EXPORT ${AREG_PACKAGE_NAME} DESTINATION share/${AREG_PACKAGE_NAME} NAMESPACE ${AREG_PACKAGE_NAME}:: FILE areglogger.cmake EXPORT_LINK_INTERFACE_LIBRARIES)
