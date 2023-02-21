
# ###########################################################################
# User sepcific settings
# ###########################################################################
include("${AREG_BASE}/areg/CMakeLists.txt")
include("${AREG_BASE}/extensions/CMakeLists.txt")
include("${AREG_BASE}/mcrouter/CMakeLists.txt")

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

target_link_libraries(areg PRIVATE ${AREG_LDFLAGS})
# Library configs
set_target_properties(areg PROPERTIES CXX_STANDARD ${AREG_CXX_STANDARD} CXX_STANDARD_REQUIRED ON )

# build areg extensions static library
addStaticLib(areg-extensions "${extensions_SRC}")

# build mcrouter executable
addExecutableEx(mcrouter "${mcrouter_SRC}" "${AREG_EXTENSIONS_LIBS}")

# copying log and router init files to 'bin/config'
file(MAKE_DIRECTORY ${AREG_OUTPUT_BIN}/config)
file(COPY ${AREG_BASE}/areg/resources/log.init DESTINATION ${AREG_OUTPUT_BIN}/config)
file(COPY ${AREG_BASE}/areg/resources/router.init DESTINATION ${AREG_OUTPUT_BIN}/config)

# add_custom_command(TARGET areg POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${AREG_BASE}/areg/resources/log.ini ${AREG_OUTPUT_BIN}/config/log.ini)
# add_custom_command(TARGET areg POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${AREG_BASE}/areg/resources/router.ini ${AREG_OUTPUT_BIN}/config/router.ini)
