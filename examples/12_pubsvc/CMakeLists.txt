set(project_root "${AREG_EXAMPLES}/12_pubsvc")

# Project names
DECLARE_PROJECT(12_generated    "generated")
DECLARE_PROJECT(12_pubclient    "pubclient")
DECLARE_PROJECT(12_pubservice   "pubservice")

addStaticLib(${project_generated}       "${generated_src}")
addExecutableEx(${project_pubclient}    "${pubclient_src}"     "${project_generated}")
addExecutableEx(${project_pubservice}   "${pubservice_src}"    "${project_generated}")

target_include_directories(${project_generated} BEFORE PRIVATE ${project_root})
target_include_directories(${project_pubclient} BEFORE PRIVATE ${project_root})
target_include_directories(${project_pubservice} BEFORE PRIVATE ${project_root})
