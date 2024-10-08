
# add service interface
addServiceInterface(23_generated res PubSubMix)

# add projects
macro_declare_project(23_common     "common")
macro_declare_project(23_pubsubctrl "pubsubctrl")
macro_declare_project(23_pubsubdyn  "pubsubdyn")

# add static library
addStaticLibEx(${project_common}       	"" "${common_src}"     23_generated)

# add executable
addExecutableEx(${project_pubsubctrl}   "" "${pubsubctrl_src}" 23_generated "${project_common}")
addExecutableEx(${project_pubsubdyn}   	"" "${pubsubdyn_src}"  23_generated "${project_common}")
