# SRC lists
unset(19_generated_SRC)
unset(19_SRC1)
unset(19_SRC2)

# Project names
set(gen_projName 19_generated)
set(projName1 19_pubclient)
set(projName2 19_pubservice)

set(19_pubwatchdog_BASE ${AREG_EXAMPLES}/19_pubwatchdog)

include(${19_pubwatchdog_BASE}/generated/CMakeLists.txt)
include(${19_pubwatchdog_BASE}/pubclient/CMakeLists.txt)
include(${19_pubwatchdog_BASE}/pubservice/CMakeLists.txt)

add_executable(${projName1} ${19_SRC1})
add_executable(${projName2} ${19_SRC2})

# Setting up appropriate flags
setLibOpt(${gen_projName} ${19_generated_SRC})
setOpt(${projName1})
setOpt(${projName2})

target_include_directories(${gen_projName} BEFORE PRIVATE ${19_pubwatchdog_BASE})
target_include_directories(${projName1} BEFORE PRIVATE ${19_pubwatchdog_BASE})
target_include_directories(${projName2} BEFORE PRIVATE ${19_pubwatchdog_BASE})

target_link_libraries(${projName1} ${gen_projName})
target_link_libraries(${projName2} ${gen_projName})
