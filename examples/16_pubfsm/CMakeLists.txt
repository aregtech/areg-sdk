set(project_root "${CMAKE_CURRENT_LIST_DIR}")

# Project names
DECLARE_PROJECT(16_generated    "generated")
DECLARE_PROJECT(16_pubclient    "pubclient")
DECLARE_PROJECT(16_pubservice   "pubservice")

# Setting up appropriate flags
addStaticLib(${project_generated}       "${generated_src}")
addExecutableEx(${project_pubclient}    "${pubclient_src}"  "${project_generated}")
addExecutableEx(${project_pubservice}   "${pubservice_src}" "${project_generated}")

target_include_directories(${project_generated}  BEFORE PRIVATE ${project_root})
target_include_directories(${project_pubclient}  BEFORE PRIVATE ${project_root})
target_include_directories(${project_pubservice} BEFORE PRIVATE ${project_root})
