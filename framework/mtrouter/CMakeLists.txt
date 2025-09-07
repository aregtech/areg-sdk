set(mtrouter_SRC)

include("${AREG_FRAMEWORK}/mtrouter/app/private/CMakeLists.txt")
include("${AREG_FRAMEWORK}/mtrouter/service/private/CMakeLists.txt")

set(_resourses  "${AREG_FRAMEWORK}/mtrouter/resources")
set(_config     "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/config")

if ((${AREG_DEVELOP_ENV} MATCHES "Win32") AND (NOT MINGW))
    set_source_files_properties("${_resourses}/mtrouter.rc" PROPERTIES LANGUAGE RC)
    list(APPEND mtrouter_SRC "${_resourses}/mtrouter.rc")
endif()

# build mtrouter executable
addExecutableEx(mtrouter ${AREG_PACKAGE_NAME} "${mtrouter_SRC}" "")
target_compile_options(mtrouter PRIVATE "${AREG_OPT_DISABLE_WARN_TOOLS}")

# Copy 'mtrouter' service running scripts
if("${AREG_DEVELOP_ENV}" STREQUAL "Win32" OR CYGWIN OR MINGW)
    add_custom_command( TARGET mtrouter POST_BUILD
                        COMMAND ${CMAKE_COMMAND} -E copy "${_resourses}/mtrouter.service.install.bat"   "${_config}/mtrouter.service.install.bat"
                        COMMAND ${CMAKE_COMMAND} -E copy "${_resourses}/mtrouter.service.uninstall.bat" "${_config}/mtrouter.service.uninstall.bat"
                        VERBATIM)
else()
    add_custom_command( TARGET mtrouter POST_BUILD
                        COMMAND ${CMAKE_COMMAND} -E copy "${_resourses}/mtrouter.service" "${_config}/mtrouter.service"
                        VERBATIM)
endif()

unset(_resourses)
unset(_config)
