
# add service interface
addServiceInterface(19_generated res HelloWatchdog)

# add projects
macro_declare_project(19_pubclient    "pubclient")
macro_declare_project(19_pubservice   "pubservice")


# add executables
addExecutableEx(${project_pubclient}  "" "${pubclient_src}"  19_generated)
addExecutableEx(${project_pubservice} "" "${pubservice_src}" 19_generated)
