
# add service interface
addServiceInterface(26_generated examples/26_pubsubmix/services/PubSubMix.siml)

# add projects

# add static library
macro_declare_static_library(26_common  26_generated
    common/src/NECommon.cpp
    common/src/Publisher.cpp
    common/src/PubSubMixed.cpp
    common/src/Subscriber.cpp
)

# add examples
macro_declare_executable(26_pubsubctrl  26_generated 26_common
    pubsubctrl/src/main.cpp
    pubsubctrl/src/PubSubController.cpp
)

macro_declare_executable(26_pubsubdyn   26_generated 26_common
    pubsubdyn/src/main.cpp
)
