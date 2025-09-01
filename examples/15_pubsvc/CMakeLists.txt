
# add service interface
addServiceInterface(15_generated examples/15_pubsvc/services/HelloWorld.siml)

# add project
macro_declare_executable(15_pubclient   15_generated 
    pubclient/src/main.cpp
    pubclient/src/ServiceClient.cpp
)

# add project
macro_declare_executable(15_pubservice  15_generated 
    pubservice/src/main.cpp
    pubservice/src/ServicingComponent.cpp
)
