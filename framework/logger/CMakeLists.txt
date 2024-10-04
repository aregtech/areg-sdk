set(logger_BASE "${AREG_BASE}/logger")
set(logger_RESOURCE "${logger_BASE}/resources")
set(logger_SRC)

include("${logger_BASE}/app/private/CMakeLists.txt")
include("${logger_BASE}/service/private/CMakeLists.txt")

# build logger executable
addExecutable(logger "${logger_SRC}")
target_compile_options(logger PRIVATE "${AREG_OPT_DISABLE_WARN_TOOLS}")
set_target_properties(logger PROPERTIES VERSION ${AREG_PROJECT_VERSION})

if("${AREG_DEVELOP_ENV}" STREQUAL "Win32" OR CYGWIN)
    add_custom_command( TARGET logger POST_BUILD
                        COMMAND ${CMAKE_COMMAND} -E copy "${logger_RESOURCE}/logger.service.install.bat"   "${AREG_OUTPUT_BIN}/logger.service.install.bat"
                        COMMAND ${CMAKE_COMMAND} -E copy "${logger_RESOURCE}/logger.service.uninstall.bat" "${AREG_OUTPUT_BIN}/logger.service.uninstall.bat"
                        VERBATIM)
else()
    add_custom_command( TARGET logger POST_BUILD
                        COMMAND ${CMAKE_COMMAND} -E copy "${logger_RESOURCE}/logger.service" "${AREG_OUTPUT_BIN}/logger.service"
                        VERBATIM)
endif()
