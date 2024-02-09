
# add service interfaces
addServiceInterface(16_generated res PowerManager)
addServiceInterface(16_generated res TrafficController)

# add projects
macro_declare_project(16_pubclient    "pubclient")
macro_declare_project(16_pubservice   "pubservice")

# add executables
addExecutableEx(${project_pubclient}    "${pubclient_src}"  16_generated)
addExecutableEx(${project_pubservice}   "${pubservice_src}" 16_generated)
