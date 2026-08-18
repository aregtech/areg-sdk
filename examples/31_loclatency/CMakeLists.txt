# add service interface
addServiceInterface(31_generated examples/31_loclatency/services/LocalLatency.siml)

macro_declare_executable(31_loclatency 31_generated
    src/main.cpp
    src/LatencyOptions.cpp
    src/LatencyReport.cpp
    src/LocalLatencyProvider.cpp
    src/LocalLatencyConsumer.cpp
)
