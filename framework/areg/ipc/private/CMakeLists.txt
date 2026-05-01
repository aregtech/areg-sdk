macro_add_source(areg_SRC "${AREG_FRAMEWORK}"
	areg/ipc/private/ClientConnection.cpp
	areg/ipc/private/ClientReceiveThread.cpp
	areg/ipc/private/ClientSendThread.cpp
	areg/ipc/private/ConnectionConfiguration.cpp
	areg/ipc/private/RemoteMessageHandler.cpp
	areg/ipc/private/ConnectionConsumer.cpp
	areg/ipc/private/ConnectionProvider.cpp
	areg/ipc/private/RegistrationConsumer.cpp
	areg/ipc/private/RegistrationProvider.cpp
	areg/ipc/private/ConnectionDefs.cpp
	areg/ipc/private/RemoteServiceDefs.cpp
	areg/ipc/private/RouterClient.cpp
	areg/ipc/private/SendMessageEvent.cpp
	areg/ipc/private/ServerConnectionBase.cpp
	areg/ipc/private/ServiceClientConnectionBase.cpp
	areg/ipc/private/ServiceEvent.cpp
	areg/ipc/private/ServiceEventConsumer.cpp
	areg/ipc/private/SocketConnectionBase.cpp
)

include("${AREG_FRAMEWORK}/areg/ipc/private/linux/CMakeLists.txt")
