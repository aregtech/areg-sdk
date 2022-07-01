# SRC lists
unset(13_generated_SRC)
unset(13_SRC1)
unset(13_SRC2)
unset(13_SRC3)

# Project names
set(gen_projName 13_generated)
set(projName1 13_pubclients)
set(projName2 13_pubsvcmain)
set(projName3 13_pubsvcmesh)

set(13_pubmesh_BASE ${AREG_EXAMPLES}/13_pubmesh)

include_directories(${13_pubmesh_BASE})

include(${13_pubmesh_BASE}/generated/CMakeLists.txt)
include(${13_pubmesh_BASE}/pubclients/CMakeLists.txt)
include(${13_pubmesh_BASE}/pubsvcmain/CMakeLists.txt)
include(${13_pubmesh_BASE}/pubsvcmesh/CMakeLists.txt)

add_executable(${projName1} ${13_SRC1})
add_executable(${projName2} ${13_SRC2})
add_executable(${projName3} ${13_SRC3})

# Setting up appropriate flags
setLibOpt(${gen_projName} ${13_generated_SRC})
setOpt(${projName1})
setOpt(${projName2})
setOpt(${projName3})

target_link_libraries(${projName1} ${gen_projName})
target_link_libraries(${projName2} ${gen_projName})
target_link_libraries(${projName3} ${gen_projName})
