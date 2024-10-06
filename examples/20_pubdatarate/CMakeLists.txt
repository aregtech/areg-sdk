
# add service interface
addServiceInterface(20_generated res LargeData)

# add projects
macro_declare_project(20_pubclient    "pubclient")
macro_declare_project(20_pubservice   "pubservice")

# add executables
addExecutableEx(${project_pubclient}  "" "${pubclient_src}"  20_generated)
addExecutableEx(${project_pubservice} "" "${pubservice_src}" 20_generated)
