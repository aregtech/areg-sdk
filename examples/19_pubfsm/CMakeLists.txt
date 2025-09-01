
# add service interfaces
addServiceInterface(19_generated examples/19_pubfsm/services/PowerManager.siml)
addServiceInterface(19_generated examples/19_pubfsm/services/TrafficController.siml)

# add projects
macro_declare_executable(19_pubclient   19_generated
    pubclient/src/main.cpp
    pubclient/src/TrafficLightClient.cpp
)

macro_declare_executable(19_pubservice  19_generated
    pubservice/src/IETrafficLightActionHandler.cpp
    pubservice/src/main.cpp
    pubservice/src/NETrafficLightFSM.cpp
    pubservice/src/PowerControllerClient.cpp
    pubservice/src/TrafficLightFSM.cpp
    pubservice/src/TrafficLightService.cpp
)

# disable some warnings in this example. They come from generated files.
target_compile_options(19_pubservice PRIVATE "${AREG_OPT_DISABLE_WARN_EXAMPLES}")
