addServiceInterface(gen_hello_test src/services/HelloService.siml)
macro_declare_executable(hello_test gen_hello_test main.cpp)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/../config/project.init" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/config/project.init" COPYONLY)
