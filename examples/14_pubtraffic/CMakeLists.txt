
#add service interface
addServiceInterface(14_generated res SimpleTrafficLight)

# add projects
macro_declare_project(14_pubclient    "pubclient")
macro_declare_project(14_pubservice   "pubservice")

# add executables
addExecutableEx(${project_pubclient}    "" "${pubclient_src}"  14_generated)
addExecutableEx(${project_pubservice}   "" "${pubservice_src}" 14_generated)
