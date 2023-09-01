# Project names
macro_declare_project(23_generated  "generated")
macro_declare_project(23_common     "common")
macro_declare_project(23_pubsubctrl "pubsubctrl")
macro_declare_project(23_pubsubdyn  "pubsubdyn")

addStaticLib(${project_generated}       "${generated_src}")
addStaticLibEx(${project_common}       	"${common_src}"     "${project_generated}")
addExecutableEx(${project_pubsubctrl}   "${pubsubctrl_src}" "${project_generated}" "${project_common}")
addExecutableEx(${project_pubsubdyn}   	"${pubsubdyn_src}"  "${project_generated}" "${project_common}")
