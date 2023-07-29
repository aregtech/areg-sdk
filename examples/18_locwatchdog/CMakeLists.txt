# Project names
macro_declare_project(18_generated    "generated")
macro_declare_project(18_locservice   "locservice")

# Setting up appropriate flags
addStaticLib(${project_generated}       "${generated_src}")
addExecutableEx(${project_locservice}   "${locservice_src}" "${project_generated}")
