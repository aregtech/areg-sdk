
# add service interface
addServiceInterface(24_generated res PubSub)

# add projects
macro_declare_project(24_publisher          "publisher")
macro_declare_project(24_subscribermulti    "subscribermulti")

# add executables
addExecutableEx(${project_publisher}   	    "" "${publisher_src}"       24_generated)
addExecutableEx(${project_subscribermulti}  "" "${subscribermulti_src}" 24_generated)
