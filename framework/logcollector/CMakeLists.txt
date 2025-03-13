set(logger_SRC)

include("${AREG_FRAMEWORK}/logcollector/app/private/CMakeLists.txt")
include("${AREG_FRAMEWORK}/logcollector/service/private/CMakeLists.txt")

set(_resourses  "${AREG_FRAMEWORK}/logcollector/resources")
set(_config     "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/config")

if (${AREG_DEVELOP_ENV} MATCHES "Win32" AND NOT MINGW)
    set_source_files_properties("${_resourses}/logcollector.rc" PROPERTIES LANGUAGE RC)
    list(APPEND logger_SRC "${_resourses}/logcollector.rc")
endif()

# build logcollector executable
addExecutableEx(logcollector ${AREG_PACKAGE_NAME} "${logger_SRC}" "")
target_compile_options(logcollector PRIVATE "${AREG_OPT_DISABLE_WARN_TOOLS}")
if (MINGW)
    target_link_options(logcollector PRIVATE -mwin32 -municode -mconsole)
endif(MINGW)

# Copy 'logcollector' service running scripts
if("${AREG_DEVELOP_ENV}" STREQUAL "Win32" OR CYGWIN OR MINGW)
    add_custom_command( TARGET logcollector POST_BUILD
                        COMMAND ${CMAKE_COMMAND} -E copy "${_resourses}/logcollector.service.install.bat"   "${_config}/logcollector.service.install.bat"
                        COMMAND ${CMAKE_COMMAND} -E copy "${_resourses}/logcollector.service.uninstall.bat" "${_config}/logcollector.service.uninstall.bat"
                        VERBATIM)
else()
    add_custom_command( TARGET logcollector POST_BUILD
                        COMMAND ${CMAKE_COMMAND} -E copy "${_resourses}/logcollector.service" "${_config}/logcollector.service"
                        VERBATIM)
endif()

unset(_resourses)
unset(_config)
