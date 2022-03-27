include("${AREG_BASE}/areg/CMakeLists.txt")
include("${AREG_BASE}/mcrouter/CMakeLists.txt")


# Adding  source files to a single executable.
add_executable(${PROJECT_NAME} ${areg_SRC} ${mcrouter_SRC})

# Setting other flags and properties
target_link_libraries(${PROJECT_NAME} m  stdc++ rt pthread)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
