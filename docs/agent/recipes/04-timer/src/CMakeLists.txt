addServiceInterface(gen_tick_timer src/services/TickService.siml)
macro_declare_executable(tick_timer gen_tick_timer main.cpp)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/../config/project.init" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/config/project.init" COPYONLY)
