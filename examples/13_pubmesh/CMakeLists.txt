# Project names
DECLARE_PROJECT(13_generated    "generated")
DECLARE_PROJECT(13_common       "common")
DECLARE_PROJECT(13_pubclients   "pubclients")
DECLARE_PROJECT(13_pubservice   "pubservice")
DECLARE_PROJECT(13_pubsvcmesh   "pubsvcmesh")

addStaticLib(${project_generated}       "${generated_src}")
addStaticLibEx(${project_common}        "${common_src}"     "${project_generated}")
addExecutableEx(${project_pubclients}   "${pubclients_src}" "${project_generated}" "${project_common}")
addExecutableEx(${project_pubservice}   "${pubservice_src}" "${project_generated}" "${project_common}")
addExecutableEx(${project_pubsvcmesh}   "${pubsvcmesh_src}" "${project_generated}" "${project_common}")
