# SRC lists
unset(SRC)
unset(11_generated_SRC)

# Project names
set(gen_projName 11_generated)
set(projName 11_locsvcmesh)

set(11_locmesh_BASE ${AREG_EXAMPLES}/11_locmesh)

# Filling SRCs up
include(${11_locmesh_BASE}/generated/CMakeLists.txt)
include(${11_locmesh_BASE}/locsvcmesh/CMakeLists.txt)

# Adding targets
add_executable(${projName} ${SRC})

# Setting up appropriate flags
setLibOpt(${gen_projName} ${11_generated_SRC})
setopt(${projName})

target_link_libraries(${projName} ${gen_projName})

target_include_directories(${gen_projName} BEFORE PRIVATE ${11_locmesh_BASE})
target_include_directories(${projName} BEFORE PRIVATE ${11_locmesh_BASE})
