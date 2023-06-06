# Project names
DECLARE_PROJECT(12_generated    "generated")
DECLARE_PROJECT(12_pubclient    "pubclient")
DECLARE_PROJECT(12_pubservice   "pubservice")

addStaticLib(${project_generated}       "${generated_src}")
addExecutableEx(${project_pubclient}    "${pubclient_src}"     "${project_generated}")
addExecutableEx(${project_pubservice}   "${pubservice_src}"    "${project_generated}")
