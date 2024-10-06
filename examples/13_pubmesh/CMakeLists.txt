# add service interfaces
addServiceInterface(13_generated res LocalHelloWorld)
addServiceInterface(13_generated res PublicHelloWorld)
addServiceInterface(13_generated res SystemShutdown)

# add projects
macro_declare_project(13_common       "common")
macro_declare_project(13_pubclients   "pubclients")
macro_declare_project(13_pubservice   "pubservice")
macro_declare_project(13_pubsvcmesh   "pubsvcmesh")

# add a static library
addStaticLibEx(${project_common}        "" "${common_src}"     13_generated)
addExecutableEx(${project_pubclients}   "" "${pubclients_src}" 13_generated "${project_common}")
addExecutableEx(${project_pubservice}   "" "${pubservice_src}" 13_generated "${project_common}")
addExecutableEx(${project_pubsvcmesh}   "" "${pubsvcmesh_src}" 13_generated "${project_common}")
