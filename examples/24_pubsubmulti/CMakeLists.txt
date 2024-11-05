
# add service interface
addServiceInterface(24_generated res PubSub)

# add projects
macro_declare_executable(24_publisher  24_generated
    publisher/src/main.cpp
    publisher/src/Publisher.cpp
)

macro_declare_executable(24_subscribermulti  24_generated
    subscribermulti/src/main.cpp
    subscribermulti/src/Subscriber.cpp
    subscribermulti/src/SubscriberBase.cpp
    subscribermulti/src/SubscriberSecond.cpp
)
