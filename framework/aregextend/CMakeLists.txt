set(aregextend_SRC)

include("${AREG_BASE}/aregextend/console/private/CMakeLists.txt")
include("${AREG_BASE}/aregextend/db/private/CMakeLists.txt")
include("${AREG_BASE}/aregextend/service/private/CMakeLists.txt")

if (${AREG_DEVELOP_ENV} MATCHES "Win32")
    set_source_files_properties("${AREG_BASE}/aregextend/resources/aregextend.rc" PROPERTIES LANGUAGE RC)
    list(APPEND aregextend_SRC "${AREG_BASE}/aregextend/resources/aregextend.rc")
endif()

# build areg extended static library
addStaticLibEx(aregextend ${AREG_PACKAGE_NAME} "${aregextend_SRC}" "")
target_compile_options(aregextend PRIVATE "${AREG_OPT_DISABLE_WARN_TOOLS}")
