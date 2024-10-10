
# add service interface
addServiceInterface(23_generated res PubSubMix)

# add projects

# add static library
macro_example_declare_lib(23_common  23_generated
    common/src/NECommon.cpp
    common/src/Publisher.cpp
    common/src/PubSubMixed.cpp
    common/src/Subscriber.cpp
)

# add examples
macro_example_declare_runnable(23_pubsubctrl  23_generated 23_common
    pubsubctrl/src/main.cpp
    pubsubctrl/src/PubSubController.cpp
)

macro_example_declare_runnable(23_pubsubdyn   23_generated 23_common
    pubsubdyn/src/main.cpp
)
