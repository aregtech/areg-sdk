
# add service interface files.
addServiceInterface(00_generated res HelloService)

# add projects
macro_declare_project_ex(00_pubclient     "clientproc"    "multiprocess/clientproc")
macro_declare_project_ex(00_pubservice    "serviceproc"   "multiprocess/serviceproc")
macro_declare_project_ex(00_onethread     "onethread"     "onethread")
macro_declare_project_ex(00_twothreads    "twothreads"    "twothreads")

# add executables
addExecutableEx(${project_clientproc}   "${clientproc_src}"     00_generated)
addExecutableEx(${project_serviceproc}  "${serviceproc_src}"    00_generated)
addExecutableEx(${project_onethread}    "${onethread_src}"      00_generated)
addExecutableEx(${project_twothreads}   "${twothreads_src}"     00_generated)
