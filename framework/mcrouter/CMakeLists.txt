set(mcrouter_SRC)

include("${AREG_FRAMEWORK}/mcrouter/app/private/CMakeLists.txt")
include("${AREG_FRAMEWORK}/mcrouter/service/private/CMakeLists.txt")

set(_resourses  "${AREG_FRAMEWORK}/mcrouter/resources")
set(_config     "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/config")

if (${AREG_DEVELOP_ENV} MATCHES "Win32")
    set_source_files_properties("${_resourses}/mcrouter.rc" PROPERTIES LANGUAGE RC)
    list(APPEND mcrouter_SRC "${_resourses}/mcrouter.rc")
endif()

# build mcrouter executable
addExecutableEx(mcrouter ${AREG_PACKAGE_NAME} "${mcrouter_SRC}" "")
target_compile_options(mcrouter PRIVATE "${AREG_OPT_DISABLE_WARN_TOOLS}")

# Copy 'mcrouter' service running scripts
if("${AREG_DEVELOP_ENV}" STREQUAL "Win32" OR CYGWIN)
    add_custom_command( TARGET mcrouter POST_BUILD
                        COMMAND ${CMAKE_COMMAND} -E copy "${_resourses}/mcrouter.service.install.bat"   "${_config}/mcrouter.service.install.bat"
                        COMMAND ${CMAKE_COMMAND} -E copy "${_resourses}/mcrouter.service.uninstall.bat" "${_config}/mcrouter.service.uninstall.bat"
                        VERBATIM)
else()
    add_custom_command( TARGET mcrouter POST_BUILD
                        COMMAND ${CMAKE_COMMAND} -E copy "${_resourses}/mcrouter.service" "${_config}/mcrouter.service"
                        VERBATIM)
endif()

unset(_resourses)
unset(_config)
