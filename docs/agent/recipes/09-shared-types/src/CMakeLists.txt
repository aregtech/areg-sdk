addServiceInterface(gen_shared_types src/services/MeterService.siml)
addServiceInterface(gen_shared_types src/services/ReportService.siml)
macro_declare_executable(shared_types gen_shared_types main.cpp)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/../config/project.init" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/config/project.init" COPYONLY)
