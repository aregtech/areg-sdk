
# add service interface files.
addServiceInterface(03_generated examples/03_helloservice/services/HelloService.siml)

# add projects
macro_declare_executable(03_pubclient   03_generated 
    common/src/ClientComponent.cpp
    common/src/ServiceComponent.cpp
    multiprocess/clientproc/src/main.cpp
)

macro_declare_executable(03_pubservice  03_generated 
    common/src/ClientComponent.cpp
    common/src/ServiceComponent.cpp
    multiprocess/serviceproc/src/main.cpp
)

macro_declare_executable(03_onethread   03_generated
    common/src/ClientComponent.cpp
    common/src/ServiceComponent.cpp
    onethread/src/main.cpp
)

macro_declare_executable(03_twothreads  03_generated
    common/src/ClientComponent.cpp
    common/src/ServiceComponent.cpp
    twothreads/src/main.cpp
)
