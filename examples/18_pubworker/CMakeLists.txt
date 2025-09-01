
# add service interface
addServiceInterface(18_generated examples/18_pubworker/services/PatientInformation.siml)

# add projects
macro_declare_executable(18_pubclient   18_generated
    pubclient/src/HardwareWorkerConsumer.cpp
    pubclient/src/main.cpp
    pubclient/src/PatientClient.cpp
)

macro_declare_executable(18_pubservice  18_generated
    pubservice/src/main.cpp
    pubservice/src/PatientService.cpp
    pubservice/src/PatientServiceWorkerConsumer.cpp
)
