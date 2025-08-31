
# add service interface
addServiceInterface(14_generated examples/14_locmesh/services/HelloWorld.siml)

# add project
macro_declare_executable(14_locmesh     14_generated 
    locsvcmesh/src/ClientComponent.cpp
    locsvcmesh/src/ServiceClient.cpp
    locsvcmesh/src/ServiceHelloWorld.cpp
    locsvcmesh/src/ServicingComponents.cpp
    locsvcmesh/src/main.cpp
)
