# ###########################################################################
# Configuration file to install AREG SDK modules.
# Copyright 2022-2023 Aregtech
# ###########################################################################

if (AREG_INSTALL)

    message(STATUS "AREG: >>> Preparing AREG SDK installation settings")

    set(AREG_PACKAGE_NAME   "areg-sdk")

    # copy compiled binaries in the bin and lib directories
    install(TARGETS areg mcrouter logger logobserver areg-extend
        RUNTIME DESTINATION bin  COMPONENT Runtime      COMPONENT ${AREG_PACKAGE_NAME}
                PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ WORLD_READ GROUP_EXECUTE WORLD_EXECUTE
        LIBRARY DESTINATION bin  COMPONENT Runtime      COMPONENT ${AREG_PACKAGE_NAME}
                PERMISSIONS OWNER_READ OWNER_WRITE               GROUP_READ WORLD_READ
        ARCHIVE DESTINATION lib  COMPONENT Development  COMPONENT ${AREG_PACKAGE_NAME}
                PERMISSIONS OWNER_READ OWNER_WRITE               GROUP_READ WORLD_READ
    )

    # Copy AREG SDK all headers
    install(DIRECTORY framework/
                DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} COMPONENT Development   COMPONENT ${AREG_PACKAGE_NAME}
                FILES_MATCHING
                    PATTERN "*.h" 
                    PATTERN "*.hpp"
                    PATTERN "*.md"
                    PATTERN "logger"            EXCLUDE
                    PATTERN "logobserver/app"   EXCLUDE
                    PATTERN "mcrouter"          EXCLUDE
    )

    # Copy all CMake and MSVC configuration files.
    install(DIRECTORY conf/
                DESTINATION conf COMPONENT Development   COMPONENT ${AREG_PACKAGE_NAME}
    )
    
    # Copy all tools
    install(DIRECTORY tools/
                DESTINATION tools               COMPONENT Development   COMPONENT ${AREG_PACKAGE_NAME}
    )

    # Copy AREG SDK license
    install(FILES LICENSE.txt
                DESTINATION licenses        COMPONENT Development   COMPONENT Runtime   COMPONENT ${AREG_PACKAGE_NAME}
                    RENAME AREG.copyright
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

    # Check and install thirdparty dependencies if needed
    if (AREG_INSTALL_DEPENDS)

        # Check how SQLite was built
        if (NOT AREG_SQLITE_FOUND)
            # copy compiled binaries in the bin and lib directories
            install( TARGETS sqlite3
                ARCHIVE DESTINATION lib         COMPONENT Development   COMPONENT ${AREG_PACKAGE_NAME}
                        PERMISSIONS OWNER_READ OWNER_WRITE               GROUP_READ WORLD_READ
            )

            # Copy thirdparty sources with headers
            install(DIRECTORY thirdparty/
                        DESTINATION thirdparty COMPONENT Development   COMPONENT ${AREG_PACKAGE_NAME}
                                PATTERN "*.vcxproj*" EXCLUDE
            )
    
            # Copy thirdparty licenses
            install(FILES ${AREG_THIRDPARTY}/sqlite3/LICENSE.txt
                        DESTINATION licenses    COMPONENT Development   COMPONENT Runtime     COMPONENT ${AREG_PACKAGE_NAME}
                            RENAME SQLITE.copyright
                            PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
            )
        endif(NOT AREG_SQLITE_FOUND)

    endif(AREG_INSTALL_DEPENDS)

endif(AREG_INSTALL)
