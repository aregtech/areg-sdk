# SRC lists
unset(20_generated_SRC)
unset(20_SRC1)
unset(20_SRC2)

# Project names
set(gen_projName 20_generated)
set(projName1 20_pubclient)
set(projName2 20_pubservice)

set(20_pubdatarate_BASE ${AREG_EXAMPLES}/20_pubdatarate)

include(${20_pubdatarate_BASE}/generated/CMakeLists.txt)
include(${20_pubdatarate_BASE}/pubclient/CMakeLists.txt)
include(${20_pubdatarate_BASE}/pubservice/CMakeLists.txt)

add_executable(${projName1} ${20_SRC1})
add_executable(${projName2} ${20_SRC2})

# Setting up appropriate flags
setLibOpt(${gen_projName} ${20_generated_SRC})
setOpt(${projName1})
setOpt(${projName2})

target_include_directories(${gen_projName} BEFORE PRIVATE ${20_pubdatarate_BASE})
target_include_directories(${projName1} BEFORE PRIVATE ${20_pubdatarate_BASE})
target_include_directories(${projName2} BEFORE PRIVATE ${20_pubdatarate_BASE})

target_link_libraries(${projName1} ${gen_projName})
target_link_libraries(${projName2} ${gen_projName})
