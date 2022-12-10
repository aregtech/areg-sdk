# ###########################################################################
# Settings
# ###########################################################################

# Set the compiler here. Possible settings: "gcc", "g++", "cl", "clang++-13"
# Can set compiler full path.
set(CMAKE_CXX_COMPILER "gcc")
set(CMAKE_C_COMPILER "gcc")

# Set the AREG library type to compile.
# Set "static" for static and "shared" for shared library
set(areg "shared")

# Set build configuration here.
# Set "Debug" for debug and "Release" for release build
set(Config "Debug")
# set(Config "Release")

# Set bitless here
set(Bitness "64")

# Specify CPU platform here
set(Platform "x86_64")

# Set the areg-sdk build root folder to output files.
set(AregBuildRoot "${AREG_SDK_ROOT}")

# Set user or project specific defines here or keep empty.
# By default it is compile with enabled logging --> "-DENABLE_TRACES"
set(UserDefines "-DENABLE_TRACES")

# Set user specific includes here or keep empty
set(UserDefIncludes)

# Set user specific library paths here or keep empty
set(UserDefLibPaths)

# Set user specific library list here or keep empty
set(UserDefLibs)

# Set user specific root folder here
set(UserDefOutput "product")
