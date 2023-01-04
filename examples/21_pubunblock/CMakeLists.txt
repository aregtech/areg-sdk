# SRC lists
unset(21_generated_SRC)
unset(21_SRC1)
unset(21_SRC2)

# Project names
set(gen_projName 21_generated)
set(projName1 21_pubclient)
set(projName2 21_pubservice)

set(21_pubunblock_BASE "${AREG_EXAMPLES}/21_pubunblock")

include(${21_pubunblock_BASE}/generated/CMakeLists.txt)
include(${21_pubunblock_BASE}/pubclient/CMakeLists.txt)
include(${21_pubunblock_BASE}/pubservice/CMakeLists.txt)

# Setting up appropriate flags
addStaticLib(${gen_projName} "${21_generated_SRC}")
addExecutableEx(${projName1} "${21_SRC1}" "${gen_projName}")
addExecutableEx(${projName2} "${21_SRC2}" "${gen_projName}")

target_include_directories(${gen_projName} BEFORE PRIVATE ${21_pubunblock_BASE})
target_include_directories(${projName1} BEFORE PRIVATE ${21_pubunblock_BASE})
target_include_directories(${projName2} BEFORE PRIVATE ${21_pubunblock_BASE})
