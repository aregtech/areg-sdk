# SRC lists
unset(20_generated_SRC)
unset(20_SRC1)
unset(20_SRC2)

# Project names
set(gen_projName 20_generated)
set(projName1 20_clientlargedata)
set(projName2 20_svclargedata)

set(20_largedata_BASE ${AREG_EXAMPLES}/20_largedata)

include(${20_largedata_BASE}/generated/CMakeLists.txt)
include(${20_largedata_BASE}/pubclient/CMakeLists.txt)
include(${20_largedata_BASE}/pubservice/CMakeLists.txt)

add_executable(${projName1} ${20_SRC1})
add_executable(${projName2} ${20_SRC2})

# Setting up appropriate flags
setLibOpt(${gen_projName} ${20_generated_SRC})
setOpt(${projName1})
setOpt(${projName2})

target_include_directories(${gen_projName} BEFORE PRIVATE ${20_largedata_BASE})
target_include_directories(${projName1} BEFORE PRIVATE ${20_largedata_BASE})
target_include_directories(${projName2} BEFORE PRIVATE ${20_largedata_BASE})

target_link_libraries(${projName1} ${gen_projName})
target_link_libraries(${projName2} ${gen_projName})
