
# add service interface
addServiceInterface(15_generated res PatientInformation)

# add projects
macro_example_declare_runnable(15_pubclient   15_generated
    pubclient/src/HardwareWorkerConsumer.cpp
    pubclient/src/main.cpp
    pubclient/src/PatientClient.cpp
)

macro_example_declare_runnable(15_pubservice  15_generated
    pubservice/src/main.cpp
    pubservice/src/PatientService.cpp
    pubservice/src/PatientServiceWorkerConsumer.cpp
)
