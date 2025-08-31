# add service interface files.
addServiceInterface(01_generated examples/01_minimalrpc/services/HelloService.siml)
macro_declare_executable(01_minimalrpc 01_generated src/main.cpp)
