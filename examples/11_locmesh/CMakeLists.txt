# SRC lists
unset(SRC)
unset(11_generated_SRC)

# Project names
set(gen_projName 11_generated)
set(projName 11_locsvcmesh)

set(11_locmesh_BASE ${AREG_EXAMPLES}/11_locmesh)

include_directories(${AREG_EXAMPLES}/11_locmesh)

# Filling SRCs up
include(${11_locmesh_BASE}/generated/CMakeLists.txt)
include(${11_locmesh_BASE}/locsvcmesh/CMakeLists.txt)

# Adding targets
add_executable(${projName} ${SRC})

# Setting up appropriate flags
if($(AREG_BINARY) MATCHES "shared")
    add_library(${gen_projName} SHARED ${11_generated_SRC})

    target_compile_definitions(${projName} PRIVATE IMP_AREG_DLL)
    target_compile_options(${projName} PRIVATE ${CompileOptShared})
else()
    add_library(${gen_projName} STATIC ${11_generated_SRC})

    target_compile_definitions(${projName} PRIVATE IMP_AREG_LIB)
    target_compile_options(${projName} PRIVATE ${CompileOptStatic})
endif()

set_property(TARGET ${gen_projName} PROPERTY CXX_STANDARD 17)
set_property(TARGET ${gen_projName} PROPERTY ARCHIVE_OUTPUT_DIRECTORY ${ProjLibDir})

target_link_libraries(${projName} ${exampleLDFlags} ${LDFLAGS} ${gen_projName})
set_target_properties(${projName}
    PROPERTIES
    CXX_STANDARD ${exampleCXXStandard}
    CXX_STANDARD_REQUIRED ON
    )
