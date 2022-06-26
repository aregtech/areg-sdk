
# SRC lists
unset(SRC)
unset(10_generated_SRC)

# Project names
set(gen_projName 10_generated)
set(projName 10_locsvc_locservice)

set(10_locsvc_BASE ${AREG_EXAMPLES}/10_locsvc)

include_directories(${AREG_EXAMPLES}/10_locsvc)

# Filling SRCs up
include(${10_locsvc_BASE}/generated/CMakeLists.txt)
include(${AREG_EXAMPLES}/10_locsvc/locservice/CMakeLists.txt)

# Adding targets
add_executable(${projName} ${SRC})

# Setting up appropriate flags
if($(AREG_BINARY) MATCHES "shared")
    add_library(${gen_projName} SHARED ${10_generated_SRC})

    target_compile_definitions(${projName} PRIVATE IMP_AREG_DLL)
    target_compile_options(${projName} PRIVATE ${CompileOptShared})
else()
    add_library(${gen_projName} STATIC ${10_generated_SRC})

    target_compile_definitions(${projName} PRIVATE IMP_AREG_LIB)
    target_compile_options(${projName} PRIVATE ${CompileOptStatic})
endif()

set_property(TARGET ${gen_projName} PROPERTY CXX_STANDARD 17)
set_property(TARGET 10_generated PROPERTY ARCHIVE_OUTPUT_DIRECTORY ${ProjLibDir})
message(${ProjLibDir})

target_link_libraries(${projName} ${exampleLDFlags} ${LDFLAGS} ${gen_projName})
set_target_properties(${projName}
    PROPERTIES
    CXX_STANDARD ${exampleCXXStandard}
    CXX_STANDARD_REQUIRED ON
    )
