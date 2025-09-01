# add service interface
addServiceInterface(25_generated examples/25_pubsub/services/PubSub.siml)

# add projects
macro_declare_executable(25_publisher   25_generated
    publisher/src/main.cpp
    publisher/src/Publisher.cpp
)

macro_declare_executable(25_subscriber   25_generated
    subscriber/src/main.cpp
    subscriber/src/Subscriber.cpp
)
