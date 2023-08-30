# Project names
macro_declare_project(22_generated       "generated")
macro_declare_project(22_pubsubclient    "pubsubclient")
macro_declare_project(22_pubsubservice   "pubsubservice")

addStaticLib(${project_generated}       	"${generated_src}")
addExecutableEx(${project_pubsubclient}    	"${pubsubclient_src}"     "${project_generated}")
addExecutableEx(${project_pubsubservice}   	"${pubsubservice_src}"    "${project_generated}")
