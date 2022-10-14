# SRC lists
unset(18_generated_SRC)
unset(18_SRC)

# Project names
set(gen_projName 18_generated)
set(projName 18_locwatchdog)

set(18_locwatchdog_BASE ${AREG_EXAMPLES}/18_locwatchdog)

# Filling SRCs up
include(${18_locwatchdog_BASE}/generated/CMakeLists.txt)
include(${18_locwatchdog_BASE}/locservice/CMakeLists.txt)

# Adding targets
add_executable(${projName} ${18_SRC})

# Setting up appropriate flags
setOpt(${projName})
setLibOpt(${gen_projName} ${18_generated_SRC})

target_link_libraries(${projName} ${gen_projName})

target_include_directories(${gen_projName} BEFORE PRIVATE ${18_locwatchdog_BASE})
target_include_directories(${projName} BEFORE PRIVATE ${18_locwatchdog_BASE})
