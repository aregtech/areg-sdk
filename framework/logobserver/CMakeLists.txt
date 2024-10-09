set(logobserver_SRC)

include("${AREG_BASE}/logobserver/app/private/CMakeLists.txt")

if (${AREG_DEVELOP_ENV} MATCHES "Win32")
    set_source_files_properties("${AREG_BASE}/logobserver/resources/logobserver.rc" PROPERTIES LANGUAGE RC)
    list(APPEND logobserver_SRC "${AREG_BASE}/logobserver/app/resources/logobserver.rc")
endif()

# ##################################################################
# build log observer console application
# ##################################################################
if (AREG_LOGGER_LIB MATCHES "shared")
    addExecutableEx(logobserver ${AREG_PACKAGE_NAME} "${logobserver_SRC}" areglogger)
    target_compile_definitions(logobserver PRIVATE IMP_LOGGER_DLL)
else ()
    addExecutableEx(logobserver ${AREG_PACKAGE_NAME} "${logobserver_SRC}" areglogger ${AREG_SQLITE_LIB_REF})
    target_compile_definitions(logobserver PRIVATE IMP_LOGGER_LIB)
endif()

target_compile_options(logobserver PRIVATE "${AREG_OPT_DISABLE_WARN_TOOLS}")
