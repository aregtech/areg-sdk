# Project names
macro_declare_project(13_generated    "generated")
macro_declare_project(13_common       "common")
macro_declare_project(13_pubclients   "pubclients")
macro_declare_project(13_pubservice   "pubservice")
macro_declare_project(13_pubsvcmesh   "pubsvcmesh")

addStaticLib(${project_generated}       "${generated_src}")
addStaticLibEx(${project_common}        "${common_src}"     "${project_generated}")
addExecutableEx(${project_pubclients}   "${pubclients_src}" "${project_generated}" "${project_common}")
addExecutableEx(${project_pubservice}   "${pubservice_src}" "${project_generated}" "${project_common}")
addExecutableEx(${project_pubsvcmesh}   "${pubsvcmesh_src}" "${project_generated}" "${project_common}")
