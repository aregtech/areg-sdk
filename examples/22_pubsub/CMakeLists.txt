# Project names
macro_declare_project(22_generated  "generated")
macro_declare_project(22_publisher  "publisher")
macro_declare_project(22_subscriber "subscriber")

addStaticLib(${project_generated}       "${generated_src}")
addExecutableEx(${project_publisher}   	"${publisher_src}"  "${project_generated}")
addExecutableEx(${project_subscriber}   "${subscriber_src}" "${project_generated}")
