# add service interfaces
addServiceInterface(16_generated examples/16_pubmesh/services/LocalHelloWorld.siml)
addServiceInterface(16_generated examples/16_pubmesh/services/PublicHelloWorld.siml)
addServiceInterface(16_generated examples/16_pubmesh/services/SystemShutdown.siml)

# add projects

# add static library
macro_declare_static_library(16_common  16_generated
    common/src/LocalHelloWorldClient.cpp
    common/src/LocalHelloWorldService.cpp
    common/src/PublicHelloWorldClient.cpp
    common/src/PublicHelloWorldService.cpp
)

# add examples
macro_declare_executable(16_pubclients 16_generated 16_common
    pubclients/src/main.cpp
)

macro_declare_executable(16_pubservice 16_generated 16_common
    pubservice/src/main.cpp
    pubservice/src/PublicServiceComponent.cpp
)

macro_declare_executable(16_pubsvcmesh 16_generated 16_common
    pubsvcmesh/src/main.cpp
)
