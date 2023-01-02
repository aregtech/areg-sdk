# SRC lists
unset(11_generated_SRC)
unset(11_SRC)

# Project names
set(gen_projName 11_generated)
set(projName 11_locmesh)

set(11_locmesh_BASE "${AREG_EXAMPLES}/11_locmesh")

# Filling SRCs up
include(${11_locmesh_BASE}/generated/CMakeLists.txt)
include(${11_locmesh_BASE}/locsvcmesh/CMakeLists.txt)

addStaticLib(${gen_projName} "${11_generated_SRC}")
addExecutableEx(${projName} "${11_SRC}" "${gen_projName}")

target_include_directories(${gen_projName} BEFORE PRIVATE ${11_locmesh_BASE})
target_include_directories(${projName} BEFORE PRIVATE ${11_locmesh_BASE})
