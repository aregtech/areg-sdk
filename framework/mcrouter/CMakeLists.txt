set(mcrouter_BASE "${AREG_BASE}/mcrouter")
set(mcrouter_SRC)

include("${mcrouter_BASE}/app/private/CMakeLists.txt")
include("${mcrouter_BASE}/service/private/CMakeLists.txt")

if (${AREG_DEVELOP_ENV} MATCHES "Win32")
    set_source_files_properties("${mcrouter_BASE}/resources/mcrouter.rc" PROPERTIES LANGUAGE RC)
    list(APPEND mcrouter_SRC ${mcrouter_BASE}/resources/mcrouter.rc)
endif()

# build mcrouter executable
addExecutableEx(mcrouter ${AREG_PACKAGE_NAME} "${mcrouter_SRC}" "")
target_compile_options(mcrouter PRIVATE "${AREG_OPT_DISABLE_WARN_TOOLS}")
set_target_properties(mcrouter  PROPERTIES VERSION ${AREG_PROJECT_VERSION})

if("${AREG_DEVELOP_ENV}" STREQUAL "Win32" OR CYGWIN)
    add_custom_command( TARGET mcrouter POST_BUILD
                        COMMAND ${CMAKE_COMMAND} -E copy "${mcrouter_BASE}/resources/mcrouter.service.install.bat"   "${AREG_OUTPUT_BIN}/mcrouter.service.install.bat"
                        COMMAND ${CMAKE_COMMAND} -E copy "${mcrouter_BASE}/resources/mcrouter.service.uninstall.bat" "${AREG_OUTPUT_BIN}/mcrouter.service.uninstall.bat"
                        VERBATIM)
else()
    add_custom_command( TARGET mcrouter POST_BUILD
                        COMMAND ${CMAKE_COMMAND} -E copy "${mcrouter_BASE}/resources/mcrouter.service" "${AREG_OUTPUT_BIN}/mcrouter.service"
                        VERBATIM)
endif()
