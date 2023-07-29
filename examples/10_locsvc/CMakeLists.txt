# Filling SRCs up
macro_declare_project(10_generated  "generated")
macro_declare_project(10_locservice "locservice")

addStaticLib(${project_generated}       "${generated_src}")
addExecutableEx(${project_locservice}   "${locservice_src}" "${project_generated}")
