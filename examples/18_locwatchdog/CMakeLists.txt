
# add service interface
addServiceInterface(18_generated res HelloWatchdog)

# add project
macro_declare_project(18_locservice   "locservice")

# add executable
addExecutableEx(${project_locservice}   "" "${locservice_src}" 18_generated)
