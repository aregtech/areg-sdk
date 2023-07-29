# Project names
macro_declare_project_ex(00_generated     "generated"     "generated")
macro_declare_project_ex(00_pubclient     "clientproc"    "multiprocess/clientproc")
macro_declare_project_ex(00_pubservice    "serviceproc"   "multiprocess/serviceproc")
macro_declare_project_ex(00_onethread     "onethread"     "onethread")
macro_declare_project_ex(00_twothreads    "twothreads"    "twothreads")

# Setting up appropriate flags
addStaticLib(${project_generated}       "${generated_src}")
addExecutableEx(${project_clientproc}   "${clientproc_src}"     "${project_generated}")
addExecutableEx(${project_serviceproc}  "${serviceproc_src}"    "${project_generated}")
addExecutableEx(${project_onethread}    "${onethread_src}"      "${project_generated}")
addExecutableEx(${project_twothreads}   "${twothreads_src}"     "${project_generated}")
