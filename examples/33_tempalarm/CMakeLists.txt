# add service interface
addServiceInterface(33_generated examples/33_tempalarm/services/TempAlarm.siml)

# add project
macro_declare_executable(33_monitor   33_generated
    monitor/src/main.cpp
    monitor/src/MonitorComponent.cpp
)

# add project
macro_declare_executable(33_operator  33_generated
    operator/src/main.cpp
    operator/src/OperatorComponent.cpp
)
