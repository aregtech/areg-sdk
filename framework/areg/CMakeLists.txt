set(areg_BASE "${AREG_BASE}/areg")
set(areg_SRC)

include("${areg_BASE}/appbase/private/CMakeLists.txt")
include("${areg_BASE}/base/private/CMakeLists.txt")
include("${areg_BASE}/component/private/CMakeLists.txt")
include("${areg_BASE}/ipc/private/CMakeLists.txt")
include("${areg_BASE}/persist/private/CMakeLists.txt")
include("${areg_BASE}/trace/private/CMakeLists.txt")

# build areg library
if(AREG_BINARY MATCHES "shared")
    # Shared Library
    add_library(areg SHARED ${areg_SRC})
    target_compile_definitions(areg PRIVATE EXP_AREG_DLL _USRDLL)
    if (NOT ${AREG_DEVELOP_ENV} MATCHES "Win32")
        target_compile_options(areg PRIVATE "-Bdynamic")
    endif()
    set_property(TARGET areg PROPERTY LIBRARY_OUTPUT_DIRECTORY ${AREG_OUTPUT_BIN})

else(AREG_BINARY MATCHES "static")

    # Static Library
    add_library(areg STATIC ${areg_SRC})
    target_compile_definitions(areg PRIVATE EXP_AREG_LIB _LIB)
    if (NOT ${AREG_DEVELOP_ENV} MATCHES "Win32")
        target_compile_options(areg PRIVATE "-Bstatic")
    endif()
    set_property(TARGET areg PROPERTY ARCHIVE_OUTPUT_DIRECTORY ${AREG_OUTPUT_LIB})

endif()
# add a dependency to make sure that output folders exist during build time.
add_dependencies(areg areg-dummy)

if (NOT ${AREG_DEVELOP_ENV} MATCHES "Win32")
    target_compile_options(areg PRIVATE -fPIC)
endif()
target_compile_options(areg PRIVATE "${AREG_OPT_DISABLE_WARN_COMMON}" "${AREG_OPT_DISABLE_WARN_FRAMEWORK}")

# Linker options
target_link_libraries(areg PRIVATE ${AREG_LDFLAGS})

# Library configuration
set_target_properties(areg PROPERTIES CXX_STANDARD ${AREG_CXX_STANDARD} CXX_STANDARD_REQUIRED ON )

# copying log and router init files to 'bin/config'
add_custom_command( TARGET areg POST_BUILD 
                    COMMAND ${CMAKE_COMMAND} -E make_directory ${AREG_OUTPUT_BIN}/config
                    COMMAND ${CMAKE_COMMAND} -E copy ${AREG_BASE}/areg/resources/areg.init ${AREG_OUTPUT_BIN}/config/areg.init
                    VERBATIM)
