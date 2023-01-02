# SRC lists
unset(15_generated_SRC)
unset(15_SRC1)
unset(15_SRC2)

# Project names
set(gen_projName 15_generated)
set(projName1 15_pubclient)
set(projName2 15_pubservice)

set(15_pubworker_BASE "${AREG_EXAMPLES}/15_pubworker")

include(${15_pubworker_BASE}/generated/CMakeLists.txt)
include(${15_pubworker_BASE}/pubclient/CMakeLists.txt)
include(${15_pubworker_BASE}/pubservice/CMakeLists.txt)

# Setting up appropriate flags
addStaticLib(${gen_projName} "${15_generated_SRC}")
addExecutableEx(${projName1} "${15_SRC1}" "${gen_projName}")
addExecutableEx(${projName2} "${15_SRC2}" "${gen_projName}")

target_include_directories(${gen_projName} BEFORE PRIVATE ${15_pubworker_BASE})
target_include_directories(${projName1} BEFORE PRIVATE ${15_pubworker_BASE})
target_include_directories(${projName2} BEFORE PRIVATE ${15_pubworker_BASE})
