# Filling SRCs up
DECLARE_PROJECT(10_generated  "generated")
DECLARE_PROJECT(10_locservice "locservice")

addStaticLib(${project_generated}       "${generated_src}")
addExecutableEx(${project_locservice}   "${locservice_src}" "${project_generated}")
