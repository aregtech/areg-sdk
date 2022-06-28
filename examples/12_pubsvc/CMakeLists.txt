# SRC lists
unset(12_generated_SRC)
unset(12_SRC1)
unset(12_SRC2)

# Project names
set(gen_projName 12_generated)
set(projName1 12_pubclient)
set(projName2 12_pubservice)

set(12_pubsvc_BASE ${AREG_EXAMPLES}/12_pubsvc)

include(${12_pubsvc_BASE}/generated/CMakeLists.txt)
include(${12_pubsvc_BASE}/pubclient/CMakeLists.txt)
include(${12_pubsvc_BASE}/pubservice/CMakeLists.txt)

add_executable(${projName1} ${12_SRC1})
add_executable(${projName2} ${12_SRC2})

# Setting up appropriate flags
setLibOpt(${gen_projName} ${12_generated_SRC})
setOpt(${projName1})
setOpt(${projName2})

target_include_directories(${gen_projName} BEFORE PRIVATE ${12_pubsvc_BASE})
target_include_directories(${projName1} BEFORE PRIVATE ${12_pubsvc_BASE})
target_include_directories(${projName2} BEFORE PRIVATE ${12_pubsvc_BASE})

target_link_libraries(${projName1} ${gen_projName})
target_link_libraries(${projName2} ${gen_projName})
