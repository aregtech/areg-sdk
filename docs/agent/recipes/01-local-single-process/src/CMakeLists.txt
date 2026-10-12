addServiceInterface(gen_hello_local src/services/HelloService.siml)
macro_declare_executable(hello_local gen_hello_local main.cpp)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/../config/project.init" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/config/project.init" COPYONLY)
