# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION ${CMAKE_VERSION}) # this file comes with cmake

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "C:/projects/areg-sdk/build_check/packages/googletest-src")
  file(MAKE_DIRECTORY "C:/projects/areg-sdk/build_check/packages/googletest-src")
endif()
file(MAKE_DIRECTORY
  "C:/projects/areg-sdk/build_check/packages/googletest-build"
  "C:/projects/areg-sdk/build_check/packages/googletest-subbuild/googletest-populate-prefix"
  "C:/projects/areg-sdk/build_check/packages/googletest-subbuild/googletest-populate-prefix/tmp"
  "C:/projects/areg-sdk/build_check/packages/googletest-subbuild/googletest-populate-prefix/src/googletest-populate-stamp"
  "C:/projects/areg-sdk/build_check/packages/googletest-subbuild/googletest-populate-prefix/src"
  "C:/projects/areg-sdk/build_check/packages/googletest-subbuild/googletest-populate-prefix/src/googletest-populate-stamp"
)

set(configSubDirs Debug)
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/projects/areg-sdk/build_check/packages/googletest-subbuild/googletest-populate-prefix/src/googletest-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "C:/projects/areg-sdk/build_check/packages/googletest-subbuild/googletest-populate-prefix/src/googletest-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
