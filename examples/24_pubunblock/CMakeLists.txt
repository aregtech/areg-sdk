# add service interface
addServiceInterface(24_generated examples/24_pubunblock/services/HelloUnblock.siml)

# add projects
macro_declare_executable(24_pubclient   24_generated
    pubclient/src/main.cpp
    pubclient/src/ServiceClient.cpp
)

macro_declare_executable(24_pubservice   24_generated
    pubservice/src/main.cpp
    pubservice/src/ServiceComponent.cpp
)
