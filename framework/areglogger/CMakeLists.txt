set(areglogger_SRC)

include("${AREG_FRAMEWORK}/areglogger/client/private/CMakeLists.txt")

# add .rc file to the list of sources
if(("${AREG_DEVELOP_ENV}" STREQUAL "Win32") AND (NOT MINGW))
    set_source_files_properties("${AREG_FRAMEWORK}/areglogger/resources/areglogger.rc" PROPERTIES LANGUAGE RC)
	list(APPEND areglogger_SRC ${AREG_FRAMEWORK}/areglogger/resources/areglogger.rc)
endif()

# ##################################################################
# build log observer API library
# ##################################################################
if (AREG_LOGGER_BINARY MATCHES "shared")

    # add .def file to the list of sources
    if(("${AREG_DEVELOP_ENV}" STREQUAL "Win32") AND (NOT MINGW))
	    list(APPEND areglogger_SRC "${AREG_FRAMEWORK}/areglogger/resources/areglogger.def")
    endif()

    # build log observer API shared library
    addSharedLibEx(areglogger ${AREG_PACKAGE_NAME} "${areglogger_SRC}" "${AREG_SQLITE_LIB_REF}")
    target_compile_options(areglogger PRIVATE "${AREG_OPT_DISABLE_WARN_TOOLS}")
    # log observer API shared library
    target_compile_definitions(areglogger PRIVATE EXP_LOGGER_DLL)

    # add MSVC linker option
    if (MSVC)
        target_link_options(areglogger PRIVATE "/DEF:\"${AREG_FRAMEWORK}/areglogger/resources/logobserver.def\"")
    endif()

else()

    # build log observer API static library
    addStaticLibEx(areglogger ${AREG_PACKAGE_NAME} "${areglogger_SRC}" "")
    target_compile_options(areglogger PRIVATE "${AREG_OPT_DISABLE_WARN_TOOLS}")
    # log observer API static library
    target_compile_definitions(areglogger PRIVATE EXP_LOGGER_LIB)

endif(AREG_LOGGER_BINARY MATCHES "shared")
