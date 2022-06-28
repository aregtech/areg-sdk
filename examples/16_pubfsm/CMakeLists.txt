#SRC lists
unset(16_generated_SRC)
unset(16_SRC1)
unset(16_SRC2)

# Project names
set(gen_projName 16_generated)
set(projName1 16_pubclient)
set(projName2 16_pubservice)

set(16_pubfsm_BASE ${AREG_EXAMPLES}/16_pubfsm)

include(${16_pubfsm_BASE}/generated/CMakeLists.txt)
include(${16_pubfsm_BASE}/pubclient/CMakeLists.txt)
include(${16_pubfsm_BASE}/pubservice/CMakeLists.txt)

add_executable(${projName1} ${16_SRC1})
add_executable(${projName2} ${16_SRC2})

# Setting up appropriate flags
setLibOpt(${gen_projName} ${16_generated_SRC})
setOpt(${projName1})
setOpt(${projName2})

target_link_libraries(${projName1} ${gen_projName})
target_link_libraries(${projName2} ${gen_projName})

target_include_directories(${gen_projName} BEFORE PRIVATE ${16_pubfsm_BASE})
target_include_directories(${projName1} BEFORE PRIVATE ${16_pubfsm_BASE})
target_include_directories(${projName2} BEFORE PRIVATE ${16_pubfsm_BASE})
