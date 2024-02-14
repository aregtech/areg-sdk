
# Add service interface files
addServiceInterface(10_generated res HelloWorld)

# add a project
macro_declare_project(10_locservice "locservice")

# create executable
addExecutableEx(${project_locservice}   "${locservice_src}" "10_generated")
