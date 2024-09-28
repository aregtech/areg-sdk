set(logobserver_BASE "${AREG_BASE}/logobserver")
set(logobserverapi_SRC)

include("${logobserver_BASE}/lib/CMakeLists.txt")
include("${logobserver_BASE}/app/CMakeLists.txt")

# ##################################################################
# build log observer API library
# ##################################################################
if (AREG_LOGOBSERVER_LIB MATCHES "shared")

    # build log observer API shared library
    addSharedLibEx(logobserverapi "${logobserverapi_SRC}" sqlite3)
    target_compile_options(logobserverapi PRIVATE "${AREG_OPT_DISABLE_WARN_TOOLS}")

    if (NOT ${AREG_DEVELOP_ENV} MATCHES "Win32")
        target_compile_options(logobserverapi PRIVATE -fPIC)
    endif()

    # log observer API shared library
    target_compile_definitions(logobserverapi PRIVATE EXP_LOGOBSERVER_DLL)

    # add MSVC linker option
    if (MSVC)
        target_link_options(logobserverapi PRIVATE "/DEF:${logobserverapi_RESOURCE}/logobserver.def")
    endif()

else(AREG_LOGOBSERVER_LIB MATCHES "static")
    # build log observer API static library
    addStaticLib(logobserverapi "${logobserverapi_SRC}")
    target_compile_options(logobserverapi PRIVATE "${AREG_OPT_DISABLE_WARN_TOOLS}")

    if (NOT ${AREG_DEVELOP_ENV} MATCHES "Win32")
        target_compile_options(logobserverapi PRIVATE -fPIC)
    endif()
    # log observer API static library
    target_compile_definitions(logobserverapi PRIVATE EXP_LOGOBSERVER_LIB)

endif(AREG_LOGOBSERVER_LIB MATCHES "shared")

# ##################################################################
# build log observer console application
# ##################################################################
if (AREG_LOGOBSERVER_LIB MATCHES "shared")
    addExecutableEx(logobserver "${logobserverapp_SRC}" logobserverapi)
    target_compile_definitions(logobserver PRIVATE IMP_LOGOBSERVER_DLL)
else (AREG_LOGOBSERVER_LIB MATCHES "static")
    addExecutableEx(logobserver "${logobserverapp_SRC}" logobserverapi sqlite3)
    target_compile_definitions(logobserver PRIVATE IMP_LOGOBSERVER_LIB)
endif()
target_compile_options(logobserver PRIVATE "${AREG_OPT_DISABLE_WARN_TOOLS}")
