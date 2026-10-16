addServiceInterface(gen_monitored_workers src/services/WorkerService.siml)
addServiceInterface(gen_monitored_workers src/services/MonitorService.siml)
macro_declare_executable(monitored_workers gen_monitored_workers main.cpp)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/../config/project.init" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/config/project.init" COPYONLY)
