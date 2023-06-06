set(project_root "${CMAKE_CURRENT_LIST_DIR}")

# Project names
DECLARE_PROJECT(13_generated    "generated")
DECLARE_PROJECT(13_common       "common")
DECLARE_PROJECT(13_pubclients   "pubclients")
DECLARE_PROJECT(13_pubservice   "pubservice")
DECLARE_PROJECT(13_pubsvcmesh   "pubsvcmesh")

list(APPEND 13_lib_list ${project_generated} ${project_common})

addStaticLib(${project_generated}       "${generated_src}")
addStaticLibEx(${project_common}        "${common_src}"     "${project_generated}")
addExecutableEx(${project_pubclients}   "${pubclients_src}" "${13_lib_list}")
addExecutableEx(${project_pubservice}   "${pubservice_src}" "${13_lib_list}")
addExecutableEx(${project_pubsvcmesh}   "${pubsvcmesh_src}" "${13_lib_list}")

target_include_directories(${project_generated}  BEFORE PRIVATE ${project_root})
target_include_directories(${project_common}     BEFORE PRIVATE ${project_root})
target_include_directories(${project_pubclients} BEFORE PRIVATE ${project_root})
target_include_directories(${project_pubservice} BEFORE PRIVATE ${project_root})
target_include_directories(${project_pubsvcmesh} BEFORE PRIVATE ${project_root})
