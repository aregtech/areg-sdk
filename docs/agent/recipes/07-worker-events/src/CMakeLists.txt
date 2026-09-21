addServiceInterface(gen_worker_events src/services/ScanService.siml)
macro_declare_executable(worker_events gen_worker_events main.cpp)

# The framework reads config/areg.init from the executable's own folder, and
# nothing copies it there for you.
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/../config/areg.init"
    "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/config/areg.init"
    COPYONLY)
