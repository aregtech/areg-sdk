
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
add_executable(${gen_projName} ${10_generated_SRC})
add_executable(${projName} ${SRC} ${10_generated_SRC})

# Setting up appropriate flags
if($(AREG_BINARY) MATCHES "shared")
    # Generated project
    target_compile_definitions(${gen_projName} PRIVATE IMP_AREG_DLL)
    target_compile_options(${gen_projName} PRIVATE ${CompileOptShared})

    target_compile_definitions(${projName} PRIVATE IMP_AREG_DLL)
    target_compile_options(${projName} PRIVATE ${CompileOptShared})
else()
    # Generated project
    target_compile_definitions(${gen_projName} PRIVATE IMP_AREG_LIB)
    target_compile_options(${gen_projName} PRIVATE ${CompileOptStatic})

    target_compile_definitions(${projName} PRIVATE IMP_AREG_LIB)
    target_compile_options(${projName} PRIVATE ${CompileOptStatic})
endif()

# Generated project
target_link_libraries(${gen_projName} ${exampleLDFlags} ${LDFLAGS})
set_target_properties(${gen_projName}
    PROPERTIES
    CXX_STANDARD ${exampleCXXStandard}
    CXX_STANDARD_REQUIRED ON
    )

target_link_libraries(${projName} ${exampleLDFlags} ${LDFLAGS})
set_target_properties(${projName}
    PROPERTIES
    CXX_STANDARD ${exampleCXXStandard}
    CXX_STANDARD_REQUIRED ON
    )
