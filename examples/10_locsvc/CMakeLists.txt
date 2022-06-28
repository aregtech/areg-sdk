# SRC lists
unset(10_generated_SRC)
unset(10_SRC)

# Project names
set(gen_projName 10_generated)
set(projName 10_locservice)

set(10_locsvc_BASE ${AREG_EXAMPLES}/10_locsvc)

# Filling SRCs up
include(${10_locsvc_BASE}/generated/CMakeLists.txt)
include(${AREG_EXAMPLES}/10_locsvc/locservice/CMakeLists.txt)

# Adding targets
add_executable(${projName} ${10_SRC})

# Setting up appropriate flags
setOpt(${projName})
setLibOpt(${gen_projName} ${10_generated_SRC})

target_link_libraries(${projName} ${gen_projName})

target_include_directories(${gen_projName} BEFORE PRIVATE ${10_locsvc_BASE})
target_include_directories(${projName} BEFORE PRIVATE ${10_locsvc_BASE})
