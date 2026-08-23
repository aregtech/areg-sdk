# add service interface
addServiceInterface(32_generated examples/32_pubmixed/services/MixedTraffic.siml)

# add projects
macro_declare_executable(32_pubservice  32_generated
    common/MixedDefs.cpp
    pubservice/src/main.cpp
    pubservice/src/ProviderComponent.cpp
)

macro_declare_executable(32_pubclient   32_generated
    common/MixedDefs.cpp
    pubclient/src/main.cpp
    pubclient/src/ConsumerComponent.cpp
)
