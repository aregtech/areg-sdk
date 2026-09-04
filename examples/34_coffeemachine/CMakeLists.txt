# add service interface and the state machine that implements it
addServiceInterface(34_generated examples/34_coffeemachine/services/CoffeeMachineService.siml)
addStateMachine(34_generated     examples/34_coffeemachine/services/CoffeeMachine.fsml)

# add project: the machine (provider)
macro_declare_executable(34_machine   34_generated
    machine/src/main.cpp
    machine/src/MachineComponent.cpp
)

# add project: the simulated user (consumer)
macro_declare_executable(34_user      34_generated
    user/src/main.cpp
    user/src/UserComponent.cpp
)
