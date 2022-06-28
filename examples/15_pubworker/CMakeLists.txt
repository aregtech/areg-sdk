# SRC lists
unset(15_generated_SRC)
unset(15_SRC1)
unset(15_SRC2)

# Project names
set(gen_projName 15_generated)
set(projName1 15_pubclient)
set(projName2 15_pubservice)

set(15_pubworker_BASE ${AREG_EXAMPLES}/15_pubworker)

include(${15_pubworker_BASE}/generated/CMakeLists.txt)
include(${15_pubworker_BASE}/pubclient/CMakeLists.txt)
include(${15_pubworker_BASE}/pubservice/CMakeLists.txt)

add_executable(${projName1} ${15_SRC1})
add_executable(${projName2} ${15_SRC2})

# Setting up appropriate flags
setLibOpt(${gen_projName} ${15_generated_SRC})
setOpt(${projName1})
setOpt(${projName2})

target_link_libraries(${projName1} ${gen_projName})
target_link_libraries(${projName2} ${gen_projName})

target_include_directories(${gen_projName} BEFORE PRIVATE ${15_pubworker_BASE})
target_include_directories(${projName1} BEFORE PRIVATE ${15_pubworker_BASE})
target_include_directories(${projName2} BEFORE PRIVATE ${15_pubworker_BASE})
