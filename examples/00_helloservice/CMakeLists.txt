#SRC lists
unset(generated_SRC)
unset(SRC1)
unset(SRC2)
unset(SRC3)
unset(SRC4)

# Project names
set(gen_projName 00_generated)
set(projName1 00_clientproc)
set(projName2 00_serviceproc)
set(projName3 00_onethread)
set(projName4 00_twothreads)

set(00_helloservice_BASE ${AREG_EXAMPLES}/00_helloservice)

include(${00_helloservice_BASE}/generated/CMakeLists.txt)
include(${00_helloservice_BASE}/multiprocess/clientproc/CMakeLists.txt)
include(${00_helloservice_BASE}/multiprocess/serviceproc/CMakeLists.txt)
include(${00_helloservice_BASE}/onethread/CMakeLists.txt)
include(${00_helloservice_BASE}/twothreads/CMakeLists.txt)

add_executable(${projName1} ${SRC1})
add_executable(${projName2} ${SRC2})
add_executable(${projName3} ${SRC3})
add_executable(${projName4} ${SRC4})

# Setting up appropriate flags
setLibOpt(${gen_projName} ${generated_SRC})
setopt(${projName1})
setopt(${projName2})
setopt(${projName3})
setopt(${projName4})

target_link_libraries(${projName1} ${gen_projName})
target_link_libraries(${projName2} ${gen_projName})
target_link_libraries(${projName3} ${gen_projName})
target_link_libraries(${projName4} ${gen_projName})

target_include_directories(${gen_projName} BEFORE PRIVATE ${00_helloservice_BASE})
target_include_directories(${projName1} BEFORE PRIVATE ${00_helloservice_BASE})
target_include_directories(${projName2} BEFORE PRIVATE ${00_helloservice_BASE})
target_include_directories(${projName3} BEFORE PRIVATE ${00_helloservice_BASE})
target_include_directories(${projName4} BEFORE PRIVATE ${00_helloservice_BASE})
