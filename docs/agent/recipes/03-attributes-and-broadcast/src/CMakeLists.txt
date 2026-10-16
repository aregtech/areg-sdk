addServiceInterface(gen_monitor src/services/Monitor.siml)
macro_declare_executable(monitor gen_monitor main.cpp)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/../config/project.init" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/config/project.init" COPYONLY)
