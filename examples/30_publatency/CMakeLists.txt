# add service interface
addServiceInterface(30_generated examples/30_publatency/services/Latency.siml)

macro_declare_executable(30_pubprovider 30_generated
    pubprovider/src/main.cpp
    pubprovider/src/LatencyProvider.cpp
)

macro_declare_executable(30_pubconsumer 30_generated
    pubconsumer/src/main.cpp
    pubconsumer/src/LatencyConsumer.cpp
)
