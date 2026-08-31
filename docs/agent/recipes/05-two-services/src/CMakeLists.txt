addServiceInterface(gen_meter_mesh src/services/MeterService.siml)
addServiceInterface(gen_meter_mesh src/services/ReportService.siml)
macro_declare_executable(meter_mesh gen_meter_mesh main.cpp)
