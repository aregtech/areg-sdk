#SRC lists
unset(16_generated_SRC)
unset(16_SRC1)
unset(16_SRC2)

# Project names
set(gen_projName 16_generated)
set(projName1 16_pubclient)
set(projName2 16_pubservice)

set(16_pubfsm_BASE "${AREG_EXAMPLES}/16_pubfsm")

include(${16_pubfsm_BASE}/generated/CMakeLists.txt)
include(${16_pubfsm_BASE}/pubclient/CMakeLists.txt)
include(${16_pubfsm_BASE}/pubservice/CMakeLists.txt)

# Setting up appropriate flags
addStaticLib(${gen_projName} "${16_generated_SRC}")
addExecutableEx(${projName1} "${16_SRC1}" "${gen_projName}")
addExecutableEx(${projName2} "${16_SRC2}" "${gen_projName}")

target_include_directories(${gen_projName} BEFORE PRIVATE ${16_pubfsm_BASE})
target_include_directories(${projName1} BEFORE PRIVATE ${16_pubfsm_BASE})
target_include_directories(${projName2} BEFORE PRIVATE ${16_pubfsm_BASE})
