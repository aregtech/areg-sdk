
# add service interface
addServiceInterface(12_generated examples/12_pubsvc/services/HelloWorld.siml)

# add project
macro_declare_executable(12_pubclient   12_generated 
    pubclient/src/main.cpp
    pubclient/src/ServiceClient.cpp
)

# add project
macro_declare_executable(12_pubservice  12_generated 
    pubservice/src/main.cpp
    pubservice/src/ServicingComponent.cpp
)
