# add service interface
addServiceInterface(23_generated examples/23_pubdatarate/services/LargeData.siml)

# add projects
macro_declare_executable(23_pubclient   23_generated
    pubclient/src/main.cpp
    pubclient/src/ServiceClient.cpp
)

macro_declare_executable(23_pubservice   23_generated
    pubservice/src/main.cpp
    pubservice/src/NEUtilities.cpp
    pubservice/src/ServicingComponent.cpp
)
