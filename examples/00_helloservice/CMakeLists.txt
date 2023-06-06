# Project names
DECLARE_PROJECT_EX(00_generated     "generated"     "generated")
DECLARE_PROJECT_EX(00_pubclient     "clientproc"    "multiprocess/clientproc")
DECLARE_PROJECT_EX(00_pubservice    "serviceproc"   "multiprocess/serviceproc")
DECLARE_PROJECT_EX(00_onethread     "onethread"     "onethread")
DECLARE_PROJECT_EX(00_twothreads    "twothreads"    "twothreads")

# Setting up appropriate flags
addStaticLib(${project_generated}       "${generated_src}")
addExecutableEx(${project_clientproc}   "${clientproc_src}"     "${project_generated}")
addExecutableEx(${project_serviceproc}  "${serviceproc_src}"    "${project_generated}")
addExecutableEx(${project_onethread}    "${onethread_src}"      "${project_generated}")
addExecutableEx(${project_twothreads}   "${twothreads_src}"     "${project_generated}")
