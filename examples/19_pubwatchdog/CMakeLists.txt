# Project names
DECLARE_PROJECT(19_generated    "generated")
DECLARE_PROJECT(19_pubclient    "pubclient")
DECLARE_PROJECT(19_pubservice   "pubservice")


# Setting up appropriate flags
addStaticLib(${project_generated}     "${generated_src}")
addExecutableEx(${project_pubclient}  "${pubclient_src}"  "${project_generated}")
addExecutableEx(${project_pubservice} "${pubservice_src}" "${project_generated}")
