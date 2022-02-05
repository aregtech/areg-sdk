set(AREG_OUTPUT_OBJ  "${ProjObjDir}")
set(AREG_OUTPUT_LIB  "${ProjLibDir}")
set(AREG_OUTPUT_BIN  "${ProjBinDir}")
set(AREG_INCLUDES    "${ProjIncludes}")
set(AREG_TOOLCHAIN   "${CrossCompile}${Toolset}")
set(AREG_AR          "${CrossCompile}ar")
set(AREG_OS          "${OpSystem}")
set(AREG_STATIC_LIB)


include("${CMAKE_CONFIG_DIR}/user.cmake") 

if(areg MATCHES "static")
    set(AREG_BINARY "static")
else()
    set(AREG_BINARY "shared")
endif()
