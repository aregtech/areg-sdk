# Adding sources
macro_add_source(areg_SRC "${AREG_FRAMEWORK}"
	areg/base/private/DateTime.cpp
	areg/base/private/DebugDefs.cpp
	areg/base/private/MessageEnvelope.cpp
	areg/base/private/File.cpp
	areg/base/private/FileBase.cpp
	areg/base/private/FileBuffer.cpp
	areg/base/private/Identifier.cpp
	areg/base/private/Process.cpp
	areg/base/private/BufferBase.cpp
	areg/base/private/ReadConverter.cpp
	areg/base/private/RuntimeBase.cpp
	areg/base/private/RuntimeObject.cpp
	areg/base/private/SharedBuffer.cpp
	areg/base/private/Socket.cpp
	areg/base/private/SocketAccepted.cpp
	areg/base/private/SocketClient.cpp
	areg/base/private/SocketDefs.cpp
	areg/base/private/SocketMultiplexer.cpp
	areg/base/private/SocketServer.cpp
	areg/base/private/String.cpp
	areg/base/private/SyncObject.cpp
	areg/base/private/SyncPrimitives.cpp
	areg/base/private/Thread.cpp
	areg/base/private/ThreadAddress.cpp
	areg/base/private/ThreadConsumer.cpp
	areg/base/private/ThreadLocalStorage.cpp
	areg/base/private/UtilityDefs.cpp
	areg/base/private/Version.cpp
	areg/base/private/WideString.cpp
	areg/base/private/WriteConverter.cpp
)

include("${AREG_FRAMEWORK}/areg/base/private/win32/CMakeLists.txt")
include("${AREG_FRAMEWORK}/areg/base/private/posix/CMakeLists.txt")
include("${AREG_FRAMEWORK}/areg/base/private/linux/CMakeLists.txt")
include("${AREG_FRAMEWORK}/areg/base/private/macos/CMakeLists.txt")
