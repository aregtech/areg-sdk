addServiceInterface(gen_gate_fsm src/services/GateService.siml)
addStateMachine(gen_gate_fsm src/services/Gate.fsml)
macro_declare_executable(gate_fsm gen_gate_fsm main.cpp)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/../config/project.init" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/config/project.init" COPYONLY)
