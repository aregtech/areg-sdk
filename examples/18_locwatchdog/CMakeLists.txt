set(project_root "${CMAKE_CURRENT_LIST_DIR}")

# Project names
DECLARE_PROJECT(18_generated    "generated")
DECLARE_PROJECT(18_locservice   "locservice")

# Setting up appropriate flags
addStaticLib(${project_generated}       "${generated_src}")
addExecutableEx(${project_locservice}   "${locservice_src}" "${project_generated}")

target_include_directories(${project_generated}  BEFORE PRIVATE ${project_root})
target_include_directories(${project_locservice} BEFORE PRIVATE ${project_root})
