include("${AREG_BASE}/areg/CMakeLists.txt")
include("${AREG_BASE}/mcrouter/CMakeLists.txt")

# Adding  source files to a single executable.
set(EXE mcrouter${AREG_BIN_EXT})
add_executable(${EXE} ${mcrouter_SRC})

if(AREG_BINARY MATCHES static)
    add_library(areg STATIC ${areg_SRC})
    target_compile_definitions(areg PRIVATE EXP_AREG_LIB=1)

    target_compile_definitions(${EXE} PRIVATE IMP_AREG_LIB=1)
else()
    add_library(areg SHARED ${areg_SRC})
    target_compile_definitions(areg PRIVATE EXP_AREG_DLL=1 _USRDLL)
    target_compile_options(areg PRIVATE -fPIC)

    target_compile_definitions(${EXE} PRIVATE IMP_AREG_DLL=1 _USRDLL)
    target_compile_options(${EXE} PRIVATE -fPIC)
endif()

# Library configs
set_property(TARGET areg PROPERTY CXX_STANDARD 17)

# Setting linker flags and properties
if(AREG_OS MATCHES "Windows")
    target_link_libraries(${EXE} areg)
else()
    target_link_libraries(${EXE} areg m stdc++ rt pthread)
endif()

set_target_properties(${EXE}
    PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    )
