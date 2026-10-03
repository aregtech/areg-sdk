addServiceInterface(gen_gate_fsm src/services/GateService.siml)
addStateMachine(gen_gate_fsm src/services/Gate.fsml)
macro_declare_executable(gate_fsm gen_gate_fsm main.cpp)
