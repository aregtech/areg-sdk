macro_add_source(areg_SRC "${AREG_FRAMEWORK}"
	areg/base/private/posix/CriticalSectionPosix.cpp
	areg/base/private/posix/DebugDefsPosix.cpp
	areg/base/private/posix/FilePosix.cpp
	areg/base/private/posix/MutexPosix.cpp
	areg/base/private/posix/ProcessPosix.cpp
	areg/base/private/posix/SocketDefsPosix.cpp
	areg/base/private/posix/SocketMultiplexerPosix.cpp
	areg/base/private/posix/SpinLockPosix.cpp
	areg/base/private/posix/SpinSyncEventPosix.cpp
	areg/base/private/posix/SyncLockAndWaitPosix.cpp
	areg/base/private/posix/SyncObjectPosix.cpp
	areg/base/private/posix/SyncPrimitivesPosix.cpp
	areg/base/private/posix/ThreadPosix.cpp
	areg/base/private/posix/UtilityDefsPosix.cpp
	areg/base/private/posix/WaitableEventPosix.cpp
	areg/base/private/posix/WaitableMutexPosix.cpp
	areg/base/private/posix/WaitablePosix.cpp
	areg/base/private/posix/WaitableSemaphorePosix.cpp
	areg/base/private/posix/WaitableTimerPosix.cpp
)
# SocketMultiplexer, WaitableTimer, SpinLock, Process platform implementations
# are in linux/ and macos/ subdirectories included from parent CMakeLists.txt.

