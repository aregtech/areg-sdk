
# ###########################################################################
# User sepcific settings
# ###########################################################################
include("${AREG_BASE}/areg/CMakeLists.txt")
include("${AREG_BASE}/extend/CMakeLists.txt")
include("${AREG_BASE}/mcrouter/CMakeLists.txt")
include("${AREG_BASE}/logger/CMakeLists.txt")

# build areg library
if(AREG_BINARY MATCHES "shared")
    # Shared Library
    add_library(areg SHARED ${areg_SRC})
    target_compile_definitions(areg PRIVATE EXP_AREG_DLL _USRDLL)
    if (NOT ${AREG_DEVELOP_ENV} MATCHES "Win32")
        target_compile_options(areg PRIVATE "-Bdynamic")
    endif()
    set_property(TARGET areg PROPERTY LIBRARY_OUTPUT_DIRECTORY ${AREG_OUTPUT_BIN})

else(AREG_BINARY MATCHES "shared")

    # Static Library
    add_library(areg STATIC ${areg_SRC})
    target_compile_definitions(areg PRIVATE EXP_AREG_LIB _LIB)
    if (NOT ${AREG_DEVELOP_ENV} MATCHES "Win32")
        target_compile_options(areg PRIVATE "-Bstatic")
    endif()
    set_property(TARGET areg PROPERTY ARCHIVE_OUTPUT_DIRECTORY ${AREG_OUTPUT_LIB})

endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(areg PRIVATE -fPIC)
endif()

# Linker options
target_link_libraries(areg PRIVATE ${AREG_LDFLAGS})

# Library configuration
set_target_properties(areg PROPERTIES CXX_STANDARD ${AREG_CXX_STANDARD} CXX_STANDARD_REQUIRED ON )

# build areg extended static library
addStaticLib(areg-extend "${extend_SRC}")

# build mcrouter executable
addExecutable(mcrouter "${mcrouter_SRC}")

# build logger executable
addExecutable(logger "${logger_SRC}")

# copying log and router init files to 'bin/config'
file(MAKE_DIRECTORY ${AREG_OUTPUT_BIN}/config)
file(COPY ${AREG_BASE}/areg/resources/log.init DESTINATION ${AREG_OUTPUT_BIN}/config)
file(COPY ${AREG_BASE}/areg/resources/router.init DESTINATION ${AREG_OUTPUT_BIN}/config)
file(COPY ${AREG_BASE}/areg/resources/logger.init DESTINATION ${AREG_OUTPUT_BIN}/config)

if("${AREG_DEVELOP_ENV}" STREQUAL "Win32")
    add_custom_command(TARGET mcrouter
                       POST_BUILD
                       COMMAND cmd /c "${mcrouter_RESOURCE}/mcrouter_post_build.bat" "${mcrouter_RESOURCE}" "${AREG_OUTPUT_BIN}")

    add_custom_command(TARGET logger
                       POST_BUILD
                       COMMAND cmd /c "${logger_RESOURCE}/logger_post_build.bat" "${logger_RESOURCE}" "${AREG_OUTPUT_BIN}")
endif()
