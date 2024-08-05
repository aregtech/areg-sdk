
# add service interfaces
addServiceInterface(16_generated res PowerManager)
addServiceInterface(16_generated res TrafficController)

# add projects
macro_declare_project(16_pubclient    "pubclient")
macro_declare_project(16_pubservice   "pubservice")

# add executables
addExecutableEx(${project_pubclient}    "${pubclient_src}"  16_generated)
if ((AREG_COMPILER_FAMILY STREQUAL "llvm") AND (AREG_OS STREQUAL "Windows"))
    target_compile_options(${project_pubclient} PRIVATE -Wno-extra-semi-stmt)
endif()

addExecutableEx(${project_pubservice}   "${pubservice_src}" 16_generated)
if ((AREG_COMPILER_FAMILY STREQUAL "llvm") AND (AREG_OS STREQUAL "Windows"))
    target_compile_options(${project_pubservice} PRIVATE -Wno-extra-semi-stmt)
endif()
