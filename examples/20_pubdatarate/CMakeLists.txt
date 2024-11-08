# add service interface
addServiceInterface(20_generated examples/20_pubdatarate/services/LargeData.siml)

# add projects
macro_declare_executable(20_pubclient   20_generated
    pubclient/src/main.cpp
    pubclient/src/ServiceClient.cpp
)

macro_declare_executable(20_pubservice   20_generated
    pubservice/src/main.cpp
    pubservice/src/NEUtilities.cpp
    pubservice/src/ServicingComponent.cpp
)
