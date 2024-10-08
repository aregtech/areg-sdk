
# add service interface
addServiceInterface(22_generated res PubSub)

# add projects
macro_declare_project(22_publisher  "publisher")
macro_declare_project(22_subscriber "subscriber")

# add executables
addExecutableEx(${project_publisher}   	"" "${publisher_src}"  22_generated)
addExecutableEx(${project_subscriber}   "" "${subscriber_src}" 22_generated)
