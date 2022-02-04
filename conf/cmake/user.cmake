
set(Toolset "g++")
set(CrossCompile)

# ###########################################################################
# Settings
# ###########################################################################

set(areg "shared")
set(bit)


set(Config "Release")


set(Platform)
set(OpSystem)

if(NOT DEFINED Platform)
    set(Platform x86_64)
endif()
if(NOT DEFINED OpSystem)
    set(OpSystem ${CMAKE_HOST_SYSTEM_NAME})
endif()


set(AregRoot ${AREG_SDK_ROOT})

set(AregInclude ${AREG_BASE})



set(UserDefines "-DENABLE_TRACES")

set(UserDefIncludes)

set(UserDefLibPaths)

set(UserDefLibs)

set(UserDefOutput)




set(ProjBuildPath)

set(ProjOutputDir "${AregRoot}/${UserDefOutput}/${ProjBuildPath}")

set(ProGenDir "${AregRoot}/${userDefOutput}/generate")

set(ProObjDir "${ProjOutputDir}/obj")

set(ProLibDir "${ProjOutputDir}/lib")

set(ProBinDir "${ProjOutputDir}/bin")


set(ProjIncludes)
set(ProjIncludes "${ProjIncludes} -I${AregInclude}")
set(ProjIncludes "${ProjIncludes} -I${ProjGenDir}")
set(ProjIncludes "${ProjIncludes} -I${UserDefIncludes}")




