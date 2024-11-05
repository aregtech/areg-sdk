
# add service interface
addServiceInterface(15_generated res PatientInformation)

# add projects
macro_declare_executable(15_pubclient   15_generated
    pubclient/src/HardwareWorkerConsumer.cpp
    pubclient/src/main.cpp
    pubclient/src/PatientClient.cpp
)

macro_declare_executable(15_pubservice  15_generated
    pubservice/src/main.cpp
    pubservice/src/PatientService.cpp
    pubservice/src/PatientServiceWorkerConsumer.cpp
)
