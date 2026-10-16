addStateMachine(gen_lamp_fsm src/services/Lamp.fsml)
macro_declare_executable(lamp_fsm gen_lamp_fsm main.cpp)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/../config/project.init" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/config/project.init" COPYONLY)
