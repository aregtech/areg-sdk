set(areg_SRC)

include("${AREG_FRAMEWORK}/areg/appbase/private/CMakeLists.txt")
include("${AREG_FRAMEWORK}/areg/base/private/CMakeLists.txt")
include("${AREG_FRAMEWORK}/areg/component/private/CMakeLists.txt")
include("${AREG_FRAMEWORK}/areg/ipc/private/CMakeLists.txt")
include("${AREG_FRAMEWORK}/areg/persist/private/CMakeLists.txt")
include("${AREG_FRAMEWORK}/areg/logging/private/CMakeLists.txt")

if ((${AREG_DEVELOP_ENV} MATCHES "Win32") AND (NOT MINGW))
    set_source_files_properties("${AREG_FRAMEWORK}/areg/resources/areg.rc" PROPERTIES LANGUAGE RC)
    list(APPEND areg_SRC ${AREG_FRAMEWORK}/areg/resources/areg.rc)
endif()

# build areg library
if(AREG_LIB_TYPE MATCHES "shared")
    # Shared Library
    add_library(areg SHARED ${areg_SRC})
    if (NOT "${AREG_PACKAGE_NAME}" STREQUAL "")
        add_library(${AREG_PACKAGE_NAME}::areg ALIAS areg)
    endif()

    target_compile_definitions(areg PRIVATE EXP_AREG_DLL _USRDLL)
    if ((NOT ${AREG_DEVELOP_ENV} MATCHES "Win32") OR (MINGW))
        target_compile_options(areg PRIVATE "-Bdynamic")
    endif()

else()

    # Static Library
    add_library(areg STATIC ${areg_SRC})
    if (NOT "${AREG_PACKAGE_NAME}" STREQUAL "")
        add_library(${AREG_PACKAGE_NAME}::areg ALIAS areg)
    endif()

    target_compile_definitions(areg PRIVATE EXP_AREG_LIB _LIB)
    if ((NOT ${AREG_DEVELOP_ENV} MATCHES "Win32") OR (MINGW))
        target_compile_options(areg PRIVATE "-Bstatic")
    endif()
    # set_property(TARGET areg PROPERTY ARCHIVE_OUTPUT_DIRECTORY ${AREG_LIB_DIR})

endif(AREG_LIB_TYPE MATCHES "shared")

if (NOT ${AREG_DEVELOP_ENV} MATCHES "Win32" OR MINGW)
    target_compile_options(areg PRIVATE -fPIC)
endif()
target_compile_options(areg PRIVATE "${AREG_OPT_DISABLE_WARN_COMMON}" "${AREG_OPT_DISABLE_WARN_FRAMEWORK}")

# Linker options
target_link_libraries(areg PRIVATE ${AREG_LDFLAGS})

# Copying the framework configuration file to 'bin/config'.
# This is an OUTPUT rule and not a POST_BUILD step on purpose: a POST_BUILD step runs
# only when the target is relinked, so editing 'areg.init' alone never reached the build
# output and the binaries silently kept the previous configuration. With OUTPUT + DEPENDS
# the copy is refreshed whenever the source file is newer than the copy, and
# 'copy_if_different' leaves the destination untouched when the contents are equal.
set(AREG_INIT_SOURCE "${AREG_FRAMEWORK}/areg/resources/areg.init")
set(AREG_INIT_TARGET "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/config/areg.init")
add_custom_command( OUTPUT  "${AREG_INIT_TARGET}"
                    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/config"
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${AREG_INIT_SOURCE}" "${AREG_INIT_TARGET}"
                    DEPENDS "${AREG_INIT_SOURCE}"
                    COMMENT "AREG: updating '${AREG_INIT_TARGET}'"
                    VERBATIM)
add_custom_target(areg-config ALL DEPENDS "${AREG_INIT_TARGET}")
add_dependencies(areg areg-config)
