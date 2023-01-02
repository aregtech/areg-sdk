# SRC lists
unset(18_generated_SRC)
unset(18_SRC)

# Project names
set(gen_projName 18_generated)
set(projName 18_locservice)

set(18_locwatchdog_BASE "${AREG_EXAMPLES}/18_locwatchdog")

# Filling SRCs up
include(${18_locwatchdog_BASE}/generated/CMakeLists.txt)
include(${18_locwatchdog_BASE}/locservice/CMakeLists.txt)

# Setting up appropriate flags
addStaticLib(${gen_projName} "${18_generated_SRC}")
addExecutableEx(${projName} "${18_SRC}" "${gen_projName}")

target_include_directories(${gen_projName} BEFORE PRIVATE ${18_locwatchdog_BASE})
target_include_directories(${projName} BEFORE PRIVATE ${18_locwatchdog_BASE})
