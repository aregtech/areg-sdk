# add service interface
addServiceInterface(21_generated examples/21_locwatchdog/services/HelloWatchdog.siml)

# add project
macro_declare_executable(21_locservice   21_generated
    locservice/src/main.cpp
    locservice/src/ServicingComponent.cpp
    locservice/src/ServiceClient.cpp
)
