set(areg_SRC)

include("${AREG_FRAMEWORK}/areg/appbase/private/CMakeLists.txt")
include("${AREG_FRAMEWORK}/areg/base/private/CMakeLists.txt")
include("${AREG_FRAMEWORK}/areg/component/private/CMakeLists.txt")
include("${AREG_FRAMEWORK}/areg/ipc/private/CMakeLists.txt")
include("${AREG_FRAMEWORK}/areg/persist/private/CMakeLists.txt")
include("${AREG_FRAMEWORK}/areg/logging/private/CMakeLists.txt")

if (${AREG_DEVELOP_ENV} MATCHES "Win32" AND NOT MINGW)
    set_source_files_properties("${AREG_FRAMEWORK}/areg/resources/areg.rc" PROPERTIES LANGUAGE RC)
    list(APPEND areg_SRC ${AREG_FRAMEWORK}/areg/resources/areg.rc)
endif()

# build areg library
if(AREG_BINARY MATCHES "shared")
    # Shared Library
    add_library(areg SHARED ${areg_SRC})
    if (NOT "${AREG_PACKAGE_NAME}" STREQUAL "")
        add_library(${AREG_PACKAGE_NAME}::areg ALIAS areg)
    endif()

    target_compile_definitions(areg PRIVATE EXP_AREG_DLL _USRDLL)
    if (NOT ${AREG_DEVELOP_ENV} MATCHES "Win32" OR MINGW)
        target_compile_options(areg PRIVATE "-Bdynamic")
    endif()

else()

    # Static Library
    add_library(areg STATIC ${areg_SRC})
    if (NOT "${AREG_PACKAGE_NAME}" STREQUAL "")
        add_library(${AREG_PACKAGE_NAME}::areg ALIAS areg)
    endif()

    target_compile_definitions(areg PRIVATE EXP_AREG_LIB _LIB)
    if (NOT ${AREG_DEVELOP_ENV} MATCHES "Win32")
        target_compile_options(areg PRIVATE "-Bstatic")
    endif()
    # set_property(TARGET areg PROPERTY ARCHIVE_OUTPUT_DIRECTORY ${AREG_OUTPUT_LIB})

endif(AREG_BINARY MATCHES "shared")

if (NOT ${AREG_DEVELOP_ENV} MATCHES "Win32" OR MINGW)
    target_compile_options(areg PRIVATE -fPIC)
endif()
target_compile_options(areg PRIVATE "${AREG_OPT_DISABLE_WARN_COMMON}" "${AREG_OPT_DISABLE_WARN_FRAMEWORK}")

# Linker options
target_link_libraries(areg PRIVATE ${AREG_LDFLAGS})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(areg)
endif()

# copying log and router init files to 'bin/config'
add_custom_command( TARGET areg POST_BUILD 
                    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/config
                    COMMAND ${CMAKE_COMMAND} -E copy ${AREG_FRAMEWORK}/areg/resources/areg.init ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/config/areg.init
                    VERBATIM)
