
# add service interface
addServiceInterface(11_generated res HelloWorld)

# add project
macro_example_declare_runnable(11_locmesh     11_generated 
    locsvcmesh/src/ClientComponent.cpp
    locsvcmesh/src/ServiceClient.cpp
    locsvcmesh/src/ServiceHelloWorld.cpp
    locsvcmesh/src/ServicingComponents.cpp
    locsvcmesh/src/main.cpp
)
