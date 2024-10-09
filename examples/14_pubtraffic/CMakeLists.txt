
#add service interface
addServiceInterface(14_generated res SimpleTrafficLight)

# add projects
macro_example_declare_runnable(14_pubclient   14_generated
    pubclient/src/main.cpp
    pubclient/src/TrafficLightClient.cpp
)

macro_example_declare_runnable(14_pubservice  14_generated
    pubservice/src/main.cpp
    pubservice/src/TrafficLightService.cpp
)
