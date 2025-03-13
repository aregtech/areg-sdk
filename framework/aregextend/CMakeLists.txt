set(aregextend_SRC)

include("${AREG_FRAMEWORK}/aregextend/console/private/CMakeLists.txt")
include("${AREG_FRAMEWORK}/aregextend/db/private/CMakeLists.txt")
include("${AREG_FRAMEWORK}/aregextend/service/private/CMakeLists.txt")

if (${AREG_DEVELOP_ENV} MATCHES "Win32" AND NOT MINGW)
    set_source_files_properties("${AREG_FRAMEWORK}/aregextend/resources/aregextend.rc" PROPERTIES LANGUAGE RC)
    list(APPEND aregextend_SRC "${AREG_FRAMEWORK}/aregextend/resources/aregextend.rc")
endif()

# build areg extended static library
addStaticLibEx(aregextend ${AREG_PACKAGE_NAME} "${aregextend_SRC}" "")
target_compile_options(aregextend PRIVATE "${AREG_OPT_DISABLE_WARN_TOOLS}")
if (MINGW)
    target_link_options(aregextend PRIVATE -mwin32 -municode)
endif(MINGW)
