set(logger_SRC)

include("${AREG_BASE}/logger/app/private/CMakeLists.txt")
include("${AREG_BASE}/logger/service/private/CMakeLists.txt")

set(_resourses  "${AREG_BASE}/logger/resources")
set(_config     "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/config")

if (${AREG_DEVELOP_ENV} MATCHES "Win32")
    set_source_files_properties("${_resourses}/logger.rc" PROPERTIES LANGUAGE RC)
    list(APPEND logger_SRC "${_resourses}/logger.rc")
endif()

# build logger executable
addExecutableEx(logger ${AREG_PACKAGE_NAME} "${logger_SRC}" "")
target_compile_options(logger PRIVATE "${AREG_OPT_DISABLE_WARN_TOOLS}")

# Copy 'logger' service running scripts
if("${AREG_DEVELOP_ENV}" STREQUAL "Win32" OR CYGWIN)
    add_custom_command( TARGET logger POST_BUILD
                        COMMAND ${CMAKE_COMMAND} -E copy "${_resourses}/logger.service.install.bat"   "${_config}/logger.service.install.bat"
                        COMMAND ${CMAKE_COMMAND} -E copy "${_resourses}/logger.service.uninstall.bat" "${_config}/logger.service.uninstall.bat"
                        VERBATIM)
else()
    add_custom_command( TARGET logger POST_BUILD
                        COMMAND ${CMAKE_COMMAND} -E copy "${_resourses}/logger.service" "${_config}/logger.service"
                        VERBATIM)
endif()

unset(_resourses)
unset(_config)
