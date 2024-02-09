
# add service interface
addServiceInterface(21_generated res HelloUnblock)

# add projects
macro_declare_project(21_pubclient    "pubclient")
macro_declare_project(21_pubservice   "pubservice")

# add executables
addExecutableEx(${project_pubclient}  "${pubclient_src}"  21_generated)
addExecutableEx(${project_pubservice} "${pubservice_src}" 21_generated)
