# SRC lists
unset(generated_SRC)
unset(SRC)

# Project names
set(gen_projName 10_generated)
set(projName 10_locservice)

set(10_locsvc_BASE ${AREG_EXAMPLES}/10_locsvc)

# Filling SRCs up
include(${10_locsvc_BASE}/generated/CMakeLists.txt)
include(${AREG_EXAMPLES}/10_locsvc/locservice/CMakeLists.txt)

# Adding targets
add_executable(${projName} ${SRC})

# Setting up appropriate flags
setopt(${projName})
setLibOpt(${gen_projName} ${generated_SRC})

target_link_libraries(${projName} ${gen_projName})

target_include_directories(${gen_projName} BEFORE PRIVATE ${10_locsvc_BASE})
target_include_directories(${projName} BEFORE PRIVATE ${10_locsvc_BASE})
