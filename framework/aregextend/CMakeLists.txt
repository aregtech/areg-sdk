set(extend_BASE "${AREG_BASE}/aregextend")
set(extend_SRC)

include("${extend_BASE}/console/private/CMakeLists.txt")
include("${extend_BASE}/db/private/CMakeLists.txt")
include("${extend_BASE}/service/private/CMakeLists.txt")

if (${AREG_DEVELOP_ENV} MATCHES "Win32")
    set_source_files_properties("${extend_BASE}/resources/aregextend.rc" PROPERTIES LANGUAGE RC)
    list(APPEND extend_SRC ${extend_BASE}/resources/aregextend.rc)
endif()


# build areg extended static library
addStaticLibEx(aregextend ${AREG_PACKAGE_NAME} "${extend_SRC}" "")

# set_target_properties(aregextend  PROPERTIES VERSION ${AREG_PROJECT_VERSION})
target_compile_options(aregextend PRIVATE "${AREG_OPT_DISABLE_WARN_TOOLS}")
