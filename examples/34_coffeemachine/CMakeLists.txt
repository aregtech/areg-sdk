
# add service interface and state machine
addServiceInterface(34_generated examples/34_coffeemachine/services/CoffeeMachineService.siml)
addStateMachine(34_generated     examples/34_coffeemachine/services/CoffeeMachine.fsml)

# add projects
macro_declare_executable(34_coffeemachine  34_generated
    machine/src/main.cpp
    machine/src/MachineComponent.cpp
)

macro_declare_executable(34_user           34_generated
    user/src/main.cpp
    user/src/UserComponent.cpp
)

# disable some warnings in this example. They come from generated files.
target_compile_options(34_coffeemachine PRIVATE "${AREG_OPT_DISABLE_WARN_EXAMPLES}")
target_compile_options(34_user          PRIVATE "${AREG_OPT_DISABLE_WARN_EXAMPLES}")
