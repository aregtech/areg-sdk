set(areglogger_BASE     "${AREG_BASE}/areglogger")
set(areglogger_SRC)

include("${areglogger_BASE}/client/private/CMakeLists.txt")

# add .rc file to the list of sources
if("${AREG_DEVELOP_ENV}" STREQUAL "Win32")
    set_source_files_properties("${areglogger_BASE}/resources/areglogger.rc" PROPERTIES LANGUAGE RC)
	list(APPEND areglogger_SRC ${areglogger_BASE}/resources/areglogger.rc)
endif()

# ##################################################################
# build log observer API library
# ##################################################################
if (AREG_LOGGER_LIB MATCHES "shared")

    # add .def file to the list of sources
    if("${AREG_DEVELOP_ENV}" STREQUAL "Win32")
	    list(APPEND areglogger_SRC ${areglogger_BASE}/resources/areglogger.def)
    endif()

    # build log observer API shared library
    addSharedLibEx(areglogger ${AREG_PACKAGE_NAME} "${areglogger_SRC}" ${AREG_SQLITE_LIB_REF})
    target_compile_options(areglogger PRIVATE "${AREG_OPT_DISABLE_WARN_TOOLS}")

    if (NOT ${AREG_DEVELOP_ENV} MATCHES "Win32")
        target_compile_options(areglogger PRIVATE -fPIC)
    endif()

    # log observer API shared library
    target_compile_definitions(areglogger PRIVATE EXP_LOGGER_DLL)

    # add MSVC linker option
    if (MSVC)
        target_link_options(areglogger PRIVATE "/DEF:${areglogger_BASE}/resources/logobserver.def")
    endif()

else(AREG_LOGGER_LIB MATCHES "static")
    # build log observer API static library
    addStaticLibEx(areglogger ${AREG_PACKAGE_NAME} "${areglogger_SRC}" "")
    target_compile_options(areglogger PRIVATE "${AREG_OPT_DISABLE_WARN_TOOLS}")

    if (NOT ${AREG_DEVELOP_ENV} MATCHES "Win32")
        target_compile_options(areglogger PRIVATE -fPIC)
    endif()
    # log observer API static library
    target_compile_definitions(areglogger PRIVATE EXP_LOGGER_LIB)

endif(AREG_LOGGER_LIB MATCHES "shared")
set_target_properties(areglogger    PROPERTIES VERSION ${AREG_PROJECT_VERSION})
