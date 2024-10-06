
# add service interface
addServiceInterface(12_generated res HelloWorld)

# add projects
macro_declare_project(12_pubclient    "pubclient")
macro_declare_project(12_pubservice   "pubservice")

# add executables
addExecutableEx(${project_pubclient}    "" "${pubclient_src}"  12_generated)
addExecutableEx(${project_pubservice}   "" "${pubservice_src}" 12_generated)
