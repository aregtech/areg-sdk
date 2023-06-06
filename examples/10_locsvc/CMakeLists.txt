set(project_root "${CMAKE_CURRENT_LIST_DIR}")

# Filling SRCs up
DECLARE_PROJECT(10_generated  "generated")
DECLARE_PROJECT(10_locservice "locservice")

addStaticLib(${project_generated}       "${generated_src}")
addExecutableEx(${project_locservice}   "${locservice_src}" "${project_generated}")

target_include_directories(${project_generated}  BEFORE PRIVATE ${project_root})
target_include_directories(${project_locservice} BEFORE PRIVATE ${project_root})
