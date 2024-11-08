
# add service interface
addServiceInterface(11_generated examples/11_locmesh/services/HelloWorld.siml)

# add project
macro_declare_executable(11_locmesh     11_generated 
    locsvcmesh/src/ClientComponent.cpp
    locsvcmesh/src/ServiceClient.cpp
    locsvcmesh/src/ServiceHelloWorld.cpp
    locsvcmesh/src/ServicingComponents.cpp
    locsvcmesh/src/main.cpp
)
