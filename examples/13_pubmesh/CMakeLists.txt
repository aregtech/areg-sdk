# add service interfaces
addServiceInterface(13_generated examples/13_pubmesh/services/LocalHelloWorld.siml)
addServiceInterface(13_generated examples/13_pubmesh/services/PublicHelloWorld.siml)
addServiceInterface(13_generated examples/13_pubmesh/services/SystemShutdown.siml)

# add projects

# add static library
macro_declare_static_library(13_common  13_generated
    common/src/LocalHelloWorldClient.cpp
    common/src/LocalHelloWorldService.cpp
    common/src/PublicHelloWorldClient.cpp
    common/src/PublicHelloWorldService.cpp
)

# add examples
macro_declare_executable(13_pubclients 13_generated 13_common
    pubclients/src/main.cpp
)

macro_declare_executable(13_pubservice 13_generated 13_common
    pubservice/src/main.cpp
    pubservice/src/PublicServiceComponent.cpp
)

macro_declare_executable(13_pubsvcmesh 13_generated 13_common
    pubsvcmesh/src/main.cpp
)
