include("${AREG_BASE}/areg/CMakeLists.txt")
include("${AREG_BASE}/mcrouter/CMakeLists.txt")


if(AREG_BINARY MATCHES static)
    add_library(areg STATIC ${areg_SRC} ${mcrouter_SRC})
else()
    add_library(areg SHARED ${areg_SRC} ${mcrouter_SRC})
endif()
# Library configs
set_property(TARGET areg PROPERTY CXX_STANDARD 17)
set_property(TARGET areg PROPERTY ARCHIVE_OUTPUT_DIRECTORY ${ProjLibDir})
set_property(TARGET areg PROPERTY LIBRARY_OUTPUT_DIRECTORY ${ProjBinDir})

# Adding  source files to a single executable.
set(EXE mcrouter${AREG_BIN_EXT})
add_executable(${EXE} ${mcrouter_SRC})

# Setting linker flags and properties
target_link_libraries(${EXE} areg m  stdc++ rt pthread)
set_target_properties(${EXE}
    PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    )
