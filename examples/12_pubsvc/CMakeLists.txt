# Project names
macro_declare_project(12_generated    "generated")
macro_declare_project(12_pubclient    "pubclient")
macro_declare_project(12_pubservice   "pubservice")

addStaticLib(${project_generated}       "${generated_src}")
addExecutableEx(${project_pubclient}    "${pubclient_src}"     "${project_generated}")
addExecutableEx(${project_pubservice}   "${pubservice_src}"    "${project_generated}")
