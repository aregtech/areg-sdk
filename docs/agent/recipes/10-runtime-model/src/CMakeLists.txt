addServiceInterface(gen_runtime_model src/services/HelloService.siml)
macro_declare_executable(runtime_model gen_runtime_model main.cpp)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/../config/project.init" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/config/project.init" COPYONLY)
