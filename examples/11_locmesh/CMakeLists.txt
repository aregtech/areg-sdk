# Project names
macro_declare_project(11_generated    "generated")
macro_declare_project(11_locmesh      "locsvcmesh")

addStaticLib(${project_generated}     "${generated_src}")
addExecutableEx(${project_locsvcmesh} "${locsvcmesh_src}" "${project_generated}")
