
# Add service interface files
addServiceInterface(13_generated examples/13_locsvc/services/HelloWorld.siml)

# add project
macro_declare_executable(13_locservice  13_generated 
    locservice/src/main.cpp
    locservice/src/ServiceClient.cpp
    locservice/src/ServicingComponent.cpp
)
