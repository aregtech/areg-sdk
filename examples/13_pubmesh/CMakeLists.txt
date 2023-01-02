# SRC lists
unset(13_generated_SRC)
unset(13_common_SRC)
unset(13_SRC1)
unset(13_SRC2)
unset(13_SRC3)

# Project names
set(gen_projName 13_generated)
set(common_projName 13_common)
set(projName1 13_pubclients)
set(projName2 13_pubservice)
set(projName3 13_pubsvcmesh)

set(13_pubmesh_BASE "${AREG_EXAMPLES}/13_pubmesh")

include_directories(${13_pubmesh_BASE})

include(${13_pubmesh_BASE}/generated/CMakeLists.txt)
include(${13_pubmesh_BASE}/common/CMakeLists.txt)
include(${13_pubmesh_BASE}/pubclients/CMakeLists.txt)
include(${13_pubmesh_BASE}/pubservice/CMakeLists.txt)
include(${13_pubmesh_BASE}/pubsvcmesh/CMakeLists.txt)

list(APPEND 13_lib_list ${gen_projName} ${common_projName})

addStaticLib(${gen_projName} "${13_generated_SRC}")
addStaticLibEx(${common_projName} "${13_common_SRC}" "${gen_projName}")
addExecutableEx(${projName1} "${13_SRC1}" "${13_lib_list}")
addExecutableEx(${projName2} "${13_SRC2}" "${13_lib_list}")
addExecutableEx(${projName3} "${13_SRC3}" "${13_lib_list}")

target_include_directories(${gen_projName} BEFORE PRIVATE ${13_pubmesh_BASE})
target_include_directories(${common_projName} BEFORE PRIVATE ${13_pubmesh_BASE})
target_include_directories(${projName1} BEFORE PRIVATE ${13_pubmesh_BASE})
target_include_directories(${projName2} BEFORE PRIVATE ${13_pubmesh_BASE})
target_include_directories(${projName3} BEFORE PRIVATE ${13_pubmesh_BASE})
