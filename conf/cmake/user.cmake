if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(Toolset "clang")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(Toolset "g++")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(Toolset "MSVC")
endif()
set(CrossCompile)

# ###########################################################################
# Settings
# ###########################################################################

set(areg "shared")
set(bit)


set(Config "Release")


set(Platform "x86_64")

set(OpSystem "${CMAKE_SYSTEM_NAME}")

if(OpSystem STREQUAL "Darwin")
    set(OpSystem "MacOS")
endif()

# Determining bitness by size of void pointer
# 8 bytes ==> x64 and 4 bytes ==> x86
if(NOT ${CMAKE_SIZEOF_VOID_P} MATCHES "8")
    set(Platform "x86")
endif()


set(AregRoot "${AREG_SDK_ROOT}")

set(AregInclude "${AREG_BASE}")



set(UserDefines "-DENABLE_TRACES")

set(UserDefIncludes)

set(UserDefLibPaths)

set(UserDefLibs)

set(UserDefOutput "build")




set(ProjBuildPath "build/${CrossCompile}${Toolset}/${OpSystem}-${Platform}-${Config}")

set(ProjOutputDir "${AregRoot}/${UserDefOutput}/${ProjBuildPath}")

set(ProjGenDir "${AregRoot}/${UserDefOutput}/generate")

set(ProjObjDir "${ProjOutputDir}/obj")

set(ProjLibDir "${ProjOutputDir}/lib")

set(ProjBinDir "${ProjOutputDir}/bin")


set(ProjIncludes "${ProjIncludes} -I${AregInclude}")
set(ProjIncludes "${ProjIncludes} -I${ProjGenDir}")
set(ProjIncludes "${ProjIncludes} ${UserDefIncludes}")
