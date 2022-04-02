include("${AREG_BASE}/areg/CMakeLists.txt")
include("${AREG_BASE}/mcrouter/CMakeLists.txt")


if(AREG_BINARY MATCHES static)
    add_library(areg STATIC ${areg_SRC} ${mcrouter_SRC})
else()
    add_library(areg SHARED ${areg_SRC} ${mcrouter_SRC})
endif()
# Library configs
set_property(TARGET areg PROPERTY CXX_STANDARD 17)
set_property(TARGET areg PROPERTY LIBRARY_OUTPUT_DIRECTORY ${ProLibDir})

# Adding  source files to a single executable.
add_executable(${PROJECT_NAME} ${mcrouter_SRC})

# Setting other flags and properties
target_link_libraries(${PROJECT_NAME} areg m  stdc++ rt pthread)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
