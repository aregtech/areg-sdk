set(logobserver_BASE "${AREG_BASE}/logobserver")
set(logobserver_SRC)

include("${logobserver_BASE}/app/private/CMakeLists.txt")

if (${AREG_DEVELOP_ENV} MATCHES "Win32")
    set_source_files_properties("${logobserver_BASE}/resources/logobserver.rc" PROPERTIES LANGUAGE RC)
    list(APPEND logobserver_SRC ${logobserver_BASE}/app/resources/logobserver.rc)
endif()

# ##################################################################
# build log observer console application
# ##################################################################
if (AREG_LOGGER_LIB MATCHES "shared")
    addExecutableEx(logobserver "${logobserver_SRC}" areglogger)
    target_compile_definitions(logobserver PRIVATE IMP_LOGGER_DLL)
else (AREG_LOGGER_LIB MATCHES "static")
    addExecutableEx(logobserver "${logobserver_SRC}" areglogger ${AREG_SQLITE_LIB_REF})
    target_compile_definitions(logobserver PRIVATE IMP_LOGGER_LIB)
endif()

set_target_properties(logobserver  PROPERTIES VERSION ${AREG_PROJECT_VERSION})
target_compile_options(logobserver PRIVATE "${AREG_OPT_DISABLE_WARN_TOOLS}")
