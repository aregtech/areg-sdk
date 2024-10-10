
# Add service interface files
addServiceInterface(10_generated res HelloWorld)

# add project
macro_example_declare_runnable(10_locservice  10_generated 
    locservice/src/main.cpp
    locservice/src/ServiceClient.cpp
    locservice/src/ServicingComponent.cpp
)
