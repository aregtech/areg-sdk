addServiceInterface(gen_hello_ipc src/services/HelloService.siml)
macro_declare_executable(hello_provider gen_hello_ipc provider.cpp)
macro_declare_executable(hello_consumer gen_hello_ipc consumer.cpp)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/../config/project.init" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/config/project.init" COPYONLY)
