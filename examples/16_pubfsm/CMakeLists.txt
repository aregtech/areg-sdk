
# add service interfaces
addServiceInterface(16_generated res PowerManager)
addServiceInterface(16_generated res TrafficController)

# add projects
macro_declare_executable(16_pubclient   16_generated
    pubclient/src/main.cpp
    pubclient/src/TrafficLightClient.cpp
)

macro_declare_executable(16_pubservice  16_generated
    pubservice/src/IETrafficLightActionHandler.cpp
    pubservice/src/main.cpp
    pubservice/src/NETrafficLightFSM.cpp
    pubservice/src/PowerControllerClient.cpp
    pubservice/src/TrafficLightFSM.cpp
    pubservice/src/TrafficLightService.cpp
)

# disable some warnings in this example. They come from generated files.
target_compile_options(16_pubservice PRIVATE "${AREG_OPT_DISABLE_WARN_EXAMPLES}")
