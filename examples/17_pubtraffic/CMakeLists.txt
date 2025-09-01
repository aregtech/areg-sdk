
#add service interface
addServiceInterface(17_generated examples/17_pubtraffic/services/SimpleTrafficLight.siml)

# add projects
macro_declare_executable(17_pubclient   17_generated
    pubclient/src/main.cpp
    pubclient/src/TrafficLightClient.cpp
)

macro_declare_executable(17_pubservice  17_generated
    pubservice/src/main.cpp
    pubservice/src/TrafficLightService.cpp
)
