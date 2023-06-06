set(project_root "${CMAKE_CURRENT_LIST_DIR}")

# Project names
DECLARE_PROJECT(11_generated    "generated")
DECLARE_PROJECT(11_locmesh      "locsvcmesh")

addStaticLib(${project_generated}     "${generated_src}")
addExecutableEx(${project_locsvcmesh} "${locsvcmesh_src}" "${project_generated}")

target_include_directories(${project_generated}  BEFORE PRIVATE ${project_root})
target_include_directories(${project_locsvcmesh} BEFORE PRIVATE ${project_root})
