
# add service interface files.
addServiceInterface(02_generated examples/02_minimalipc/services/HelloService.siml)

# add projects
macro_declare_executable(02_consumeripc 02_generated src/consumeripc.cpp)
macro_declare_executable(02_provideripc 02_generated src/provideripc.cpp)
