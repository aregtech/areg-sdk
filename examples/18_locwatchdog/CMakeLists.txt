# add service interface
addServiceInterface(18_generated res HelloWatchdog)

# add project
macro_example_declare_runnable(18_locservice   18_generated
    locservice/src/main.cpp
    locservice/src/ServicingComponent.cpp
    locservice/src/ServiceClient.cpp
)
