
# add service interface
addServiceInterface(11_generated res HelloWorld)

# Project names
macro_declare_project(11_locmesh      "locsvcmesh")

# create exicutable
addExecutableEx(${project_locsvcmesh} "" "${locsvcmesh_src}" 11_generated)
