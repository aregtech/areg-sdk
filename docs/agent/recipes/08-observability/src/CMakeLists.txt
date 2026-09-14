addServiceInterface(gen_observability src/services/HelloService.siml)
macro_declare_executable(obs_provider gen_observability provider.cpp)
macro_declare_executable(obs_consumer gen_observability consumer.cpp)

# Both executables read the same file from their own config/ subdirectory.
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/../config/areg.init"
    "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/config/areg.init"
    COPYONLY)
