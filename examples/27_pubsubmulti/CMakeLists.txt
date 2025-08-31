
# add service interface
addServiceInterface(27_generated examples/27_pubsubmulti/services/PubSub.siml)

# add projects
macro_declare_executable(27_publisher  27_generated
    publisher/src/main.cpp
    publisher/src/Publisher.cpp
)

macro_declare_executable(27_subscribermulti  27_generated
    subscribermulti/src/main.cpp
    subscribermulti/src/Subscriber.cpp
    subscribermulti/src/SubscriberBase.cpp
    subscribermulti/src/SubscriberSecond.cpp
)
