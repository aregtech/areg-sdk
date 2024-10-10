# add service interface
addServiceInterface(22_generated res PubSub)

# add projects
macro_example_declare_runnable(22_publisher   22_generated
    publisher/src/main.cpp
    publisher/src/Publisher.cpp
)

macro_example_declare_runnable(22_subscriber   22_generated
    subscriber/src/main.cpp
    subscriber/src/Subscriber.cpp
)
