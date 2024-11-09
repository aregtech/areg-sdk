
# Add service interface files
addServiceInterface(10_generated examples/10_locsvc/services/HelloWorld.siml)

# add project
macro_declare_executable(10_locservice  10_generated 
    locservice/src/main.cpp
    locservice/src/ServiceClient.cpp
    locservice/src/ServicingComponent.cpp
)
