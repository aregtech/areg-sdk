include("${AREG_BASE}/areg/CMakeLists.txt")
include("${AREG_BASE}/mcrouter/CMakeLists.txt")


# Adding  source files
add_executable(${PROJECT_NAME} ${areg_SRC} ${mcrouter_SRC})
target_link_libraries(${PROJECT_NAME} m  stdc++ rt pthread)
