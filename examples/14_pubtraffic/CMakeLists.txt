# SRC lists
unset(generated_SRC)
unset(SRC1)
unset(SRC2)

# Project names
set(gen_projName 14_generated)
set(projName1 14_pubclient)
set(projName2 14_pubservice)

set(14_pubtraffic_BASE ${AREG_EXAMPLES}/14_pubtraffic)

include(${14_pubtraffic_BASE}/generated/CMakeLists.txt)
include(${14_pubtraffic_BASE}/pubclient/CMakeLists.txt)
include(${14_pubtraffic_BASE}/pubservice/CMakeLists.txt)

add_executable(${projName1} ${SRC1})
add_executable(${projName2} ${SRC2})

# Setting up appropriate flags
setLibOpt(${gen_projName} ${generated_SRC})
setopt(${projName1})
setopt(${projName2})

target_link_libraries(${projName1} ${gen_projName})
target_link_libraries(${projName2} ${gen_projName})

target_include_directories(${gen_projName} BEFORE PRIVATE ${14_pubtraffic_BASE})
target_include_directories(${projName1} BEFORE PRIVATE ${14_pubtraffic_BASE} ${14_pubtraffic_BASE}/generated)
target_include_directories(${projName2} BEFORE PRIVATE ${14_pubtraffic_BASE} ${14_pubtraffic_BASE}/generated)
