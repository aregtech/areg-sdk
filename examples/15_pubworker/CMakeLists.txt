
# add service interface
addServiceInterface(15_generated res PatientInformation)

# add projects
macro_declare_project(15_pubclient    "pubclient")
macro_declare_project(15_pubservice   "pubservice")

# add executables
addExecutableEx(${project_pubclient}    "" "${pubclient_src}"  15_generated)
addExecutableEx(${project_pubservice}   "" "${pubservice_src}" 15_generated)
