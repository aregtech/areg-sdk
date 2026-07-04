# Win32 API Reference

```
This file is part of Areg SDK
Copyright (c) Aregtech (Artak Avetyan), 2017-2026
Contact: info[at]areg.tech
Website: https://www.areg.tech
```

This document describes Win32 API usage in Areg SDK for Windows platforms.

---

## Building with Win32

To build with the Win32 API, define the preprocessor symbol **WINDOWS**. See the [Preprocessor Definitions](./wiki/02f-preprocessor-definitions.md) guide for details.

### Build Options

| Method | Guide |
|--------|-------|
| CMake | [Building with CMake](./wiki/01b-cmake-build.md) |
| MSBuild | [Building with Visual Studio](./wiki/01c-msvc-build.md) |

### Dependencies

Areg SDK uses Windows Sockets and Win32 API for:
- Networking
- Multithreading
- Synchronization

When `AREG_EXTENDED` is enabled, extended console features require Windows 2000 or later.

### Build Example

```batch
:: Standard build
msbuild /m /property:Configuration=Release /property:Platform=x64 ./areg-sdk.sln

:: Build with extended features
msbuild /m /property:Configuration=Debug /property:Platform=x64 /property:AregExtended=1 ./areg-sdk.sln
```

---

## Platform Requirements

| Feature | Minimum Windows Version |
|---------|------------------------|
| Standard build | Windows XP |
| Extended features | Windows 2000 |
| Service support | Windows XP |

---

## Win32 API Functions Used

The following Win32 API functions are used in Areg SDK (including the Multitarget Router):

### Threading and Synchronization

```
CreateThread            ExitThread              GetCurrentThreadId
SetThreadPriority       SuspendThread           ResumeThread
TerminateThread         TlsAlloc                TlsFree
TlsGetValue             TlsSetValue             Sleep
SleepEx                 WaitForSingleObject     WaitForMultipleObjects
CreateEvent             SetEvent                ResetEvent
CloseHandle             CreateMutex             ReleaseMutex
InitializeCriticalSection                       DeleteCriticalSection
EnterCriticalSection                            LeaveCriticalSection
TryEnterCriticalSection                         InitializeConditionVariable
SleepConditionVariableCS                        WakeConditionVariable
WakeAllConditionVariable
```

### Socket Operations (Winsock2)

```
WSAStartup              WSACleanup              WSAGetLastError
socket                  bind                    listen
accept                  connect                 send
recv                    closesocket             shutdown
setsockopt              getsockopt              select
getaddrinfo             freeaddrinfo            inet_ntop
htons                   ntohs                   ioctlsocket
```

### File and Directory Operations

```
CreateFile              ReadFile                WriteFile
CloseHandle             GetFileSize             SetFilePointer
CreateDirectory         RemoveDirectory         DeleteFile
GetCurrentDirectory     SetCurrentDirectory     FindFirstFile
FindNextFile            FindClose               GetFileAttributes
GetTempPath             GetTempFileName
```

### Time Functions

```
GetSystemTime           GetLocalTime            SystemTimeToFileTime
FileTimeToSystemTime    GetTickCount            GetTickCount64
QueryPerformanceCounter                         QueryPerformanceFrequency
CreateWaitableTimer                             SetWaitableTimer
CancelWaitableTimer
```

### Process and Module

```
GetCurrentProcess       GetCurrentProcessId     GetModuleFileName
GetModuleHandle         GetCommandLine          GetEnvironmentVariable
```

### Memory

```
HeapAlloc               HeapFree                HeapReAlloc
VirtualAlloc            VirtualFree             GetProcessHeap
```

### Console (Extended Features)

```
GetStdHandle            SetConsoleCursorPosition
GetConsoleScreenBufferInfo                      SetConsoleTextAttribute
WriteConsole            ReadConsole             SetConsoleMode
GetConsoleMode
```

### Service Control

```
RegisterServiceCtrlHandler                      SetServiceStatus
StartServiceCtrlDispatcher                      OpenSCManager
CreateService           DeleteService           OpenService
StartService            ControlService          CloseServiceHandle
```

### String Operations

```
MultiByteToWideChar     WideCharToMultiByte     lstrcpy
lstrlen                 wsprintf                wvsprintf
```

---

## Notes

- Winsock2 must be initialized with `WSAStartup` before socket operations
- Service functions are used only when running as a Windows service
- Console functions are used only with `AREG_EXTENDED` enabled
