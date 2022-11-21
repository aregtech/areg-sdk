#SRC lists
unset(00_generated_SRC)
unset(00_SRC1)
unset(00_SRC2)
unset(00_SRC3)
unset(00_SRC4)

# Project names
set(gen_projName 00_generated)
set(projName1 00_pubclient)
set(projName2 00_pubservice)
set(projName3 00_onethread)
set(projName4 00_twothreads)

set(00_helloservice_BASE ${AREG_EXAMPLES}/00_helloservice)

include(${00_helloservice_BASE}/generated/CMakeLists.txt)
include(${00_helloservice_BASE}/multiprocess/clientproc/CMakeLists.txt)
include(${00_helloservice_BASE}/multiprocess/serviceproc/CMakeLists.txt)
include(${00_helloservice_BASE}/onethread/CMakeLists.txt)
include(${00_helloservice_BASE}/twothreads/CMakeLists.txt)

add_executable(${projName1} ${00_SRC1})
add_executable(${projName2} ${00_SRC2})
add_executable(${projName3} ${00_SRC3})
add_executable(${projName4} ${00_SRC4})

# Setting up appropriate flags
setLibOpt(${gen_projName} ${00_generated_SRC})
setOpt(${projName1})
setOpt(${projName2})
setOpt(${projName3})
setOpt(${projName4})

target_link_libraries(${projName1} ${gen_projName})
target_link_libraries(${projName2} ${gen_projName})
target_link_libraries(${projName3} ${gen_projName})
target_link_libraries(${projName4} ${gen_projName})

target_include_directories(${gen_projName} BEFORE PRIVATE ${00_helloservice_BASE})
target_include_directories(${projName1} BEFORE PRIVATE ${00_helloservice_BASE})
target_include_directories(${projName2} BEFORE PRIVATE ${00_helloservice_BASE})
target_include_directories(${projName3} BEFORE PRIVATE ${00_helloservice_BASE})
target_include_directories(${projName4} BEFORE PRIVATE ${00_helloservice_BASE})
