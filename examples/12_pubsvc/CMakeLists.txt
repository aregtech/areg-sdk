
# add service interface
addServiceInterface(12_generated res HelloWorld)

# add project
macro_example_declare_runnable(12_pubclient   12_generated 
    pubclient/src/main.cpp
    pubclient/src/ServiceClient.cpp
)

# add project
macro_example_declare_runnable(12_pubservice  12_generated 
    pubservice/src/main.cpp
    pubservice/src/ServicingComponent.cpp
)
