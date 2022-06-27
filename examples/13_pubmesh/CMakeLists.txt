# SRC lists
unset(generated_SRC)
unset(SRC1)
unset(SRC2)
unset(SRC3)

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

add_executable(${projName1} ${SRC1})
add_executable(${projName2} ${SRC2})
add_executable(${projName3} ${SRC3})

# Setting up appropriate flags
setLibOpt(${gen_projName} ${generated_SRC})
setopt(${projName1})
setopt(${projName2})
setopt(${projName3})

target_link_libraries(${projName1} ${gen_projName})
target_link_libraries(${projName2} ${gen_projName})
target_link_libraries(${projName3} ${gen_projName})
