# Project names
DECLARE_PROJECT(11_generated    "generated")
DECLARE_PROJECT(11_locmesh      "locsvcmesh")

addStaticLib(${project_generated}     "${generated_src}")
addExecutableEx(${project_locsvcmesh} "${locsvcmesh_src}" "${project_generated}")
