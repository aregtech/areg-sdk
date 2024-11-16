
# add service interface
addServiceInterface(19_generated examples/19_pubwatchdog/services/HelloWatchdog.siml)

# add projects
macro_declare_executable(19_pubclient   19_generated
    pubclient/src/main.cpp
    pubclient/src/ServiceClient.cpp
)

macro_declare_executable(19_pubservice   19_generated
    pubservice/src/main.cpp
    pubservice/src/ServicingComponent.cpp
)
