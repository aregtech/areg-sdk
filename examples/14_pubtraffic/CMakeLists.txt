# SRC lists
unset(14_generated_SRC)
unset(14_SRC1)
unset(14_SRC2)

# Project names
set(gen_projName 14_generated)
set(projName1 14_pubclient)
set(projName2 14_pubservice)

set(14_pubtraffic_BASE "${AREG_EXAMPLES}/14_pubtraffic")

include(${14_pubtraffic_BASE}/generated/CMakeLists.txt)
include(${14_pubtraffic_BASE}/pubclient/CMakeLists.txt)
include(${14_pubtraffic_BASE}/pubservice/CMakeLists.txt)

addStaticLib(${gen_projName} "${14_generated_SRC}")
addExecutableEx(${projName1} "${14_SRC1}" "${gen_projName}")
addExecutableEx(${projName2} "${14_SRC2}" "${gen_projName}")

target_include_directories(${gen_projName} BEFORE PRIVATE ${14_pubtraffic_BASE})
target_include_directories(${projName1} BEFORE PRIVATE ${14_pubtraffic_BASE})
target_include_directories(${projName2} BEFORE PRIVATE ${14_pubtraffic_BASE})
