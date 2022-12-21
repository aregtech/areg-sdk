include("${AREG_BASE}/areg/CMakeLists.txt")
include("${AREG_BASE}/mcrouter/CMakeLists.txt")

# Adding  source files to a single executable.
set(EXE mcrouter${AREG_BIN_EXT})
add_executable(${EXE} ${mcrouter_SRC})

if(AREG_BINARY MATCHES "shared")
    # Shared Library
    add_library(areg SHARED ${areg_SRC})
    target_compile_definitions(areg PRIVATE EXP_AREG_DLL)
    target_compile_definitions(areg PRIVATE _USRDLL)
    target_link_libraries(areg ${LDFLAGS})

    target_compile_definitions(${EXE} PRIVATE IMP_AREG_DLL)

    if (AREG_DEVELOP_ENV MATCHES "Posix")
        target_compile_options(areg PRIVATE -fPIC)
        target_compile_options(${EXE} PRIVATE -fPIC)
    endif()

else()
    # Static Library
    add_library(areg STATIC ${areg_SRC})
    target_compile_definitions(areg PRIVATE EXP_AREG_LIB)

    target_compile_definitions(${EXE} PRIVATE IMP_AREG_LIB)
endif()

# Library configs
set_property(TARGET areg PROPERTY CXX_STANDARD 17)

# Setting linker flags and properties
target_link_libraries(${EXE} areg ${LDFLAGS})

set_target_properties(${EXE}
    PROPERTIES
    CXX_STANDARD ${aregCXXStandard}
    CXX_STANDARD_REQUIRED ON
    )
