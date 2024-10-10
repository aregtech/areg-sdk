
# add service interface files.
addServiceInterface(00_generated res HelloService)

# add projects
macro_example_declare_runnable(00_pubclient   00_generated 
    common/src/ClientComponent.cpp
    common/src/ServiceComponent.cpp
    multiprocess/clientproc/src/main.cpp
)

macro_example_declare_runnable(00_pubservice  00_generated 
    common/src/ClientComponent.cpp
    common/src/ServiceComponent.cpp
    multiprocess/serviceproc/src/main.cpp
)

macro_example_declare_runnable(00_onethread   00_generated
    common/src/ClientComponent.cpp
    common/src/ServiceComponent.cpp
    onethread/src/main.cpp
)

macro_example_declare_runnable(00_twothreads  00_generated
    common/src/ClientComponent.cpp
    common/src/ServiceComponent.cpp
    twothreads/src/main.cpp
)
