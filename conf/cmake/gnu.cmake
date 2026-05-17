# ###########################################################################
# GNU compiler and linker options for POSIX API
# Copyright 2022-2026 Aregtech
# ###########################################################################

message(STATUS "Areg: >>> Preparing settings for GNU compiler under \'${AREG_OS}\' platform, Cygwin = \'${CYGWIN}\', MINGW = \'${MINGW}\'")

if (CYGWIN)
    set(AREG_COMPILER_FAMILY "cygwin")
elseif (MINGW)
    set(AREG_COMPILER_FAMILY "mingw")
else()
    set(AREG_COMPILER_FAMILY "gnu")
endif()

if (MINGW)

    set(AREG_DEVELOP_ENV "Win32")
    add_definitions(-DWINDOWS -D_WINDOWS -DWIN32 -D_WIN32)
    set(AREG_COMPILER_VERSION -std=c++17)
    list(APPEND AREG_COMPILER_OPTIONS -pthread -Wall -c -fmessage-length=0 -municode -MMD ${AREG_USER_DEFINES})

    if (${AREG_ARCH} STREQUAL ${_proc_x86} OR ${AREG_ARCH} STREQUAL ${_proc_x64})
        if(${AREG_BITNESS} EQUAL 32)
            list(APPEND AREG_COMPILER_OPTIONS -m32)
            list(APPEND AREG_LDFLAGS -m32)
        else()
            list(APPEND AREG_COMPILER_OPTIONS -m64)
            list(APPEND AREG_LDFLAGS -m64)
        endif()
    endif()

    if(${CMAKE_BUILD_TYPE} MATCHES "Release")
        list(APPEND AREG_COMPILER_OPTIONS -O3 -ffunction-sections -fdata-sections)
        if (NOT CMAKE_CROSSCOMPILING)
            list(APPEND AREG_COMPILER_OPTIONS -march=native -mtune=native)
        endif()
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
        list(APPEND AREG_LDFLAGS   -Wl,--gc-sections -Wl,-O1   stdc++   m   pthread   advapi32   psapi   shell32   ws2_32)
        set(AREG_LDFLAGS_STR    "-Wl,--gc-sections -Wl,-O1 -lstdc++ -lm -lpthread -ladvapi32 -lpsapi -lshell32 -lws2_32")
    else()
        list(APPEND AREG_COMPILER_OPTIONS -O0 -g3)
        list(APPEND AREG_LDFLAGS   stdc++   m   pthread   advapi32   psapi   shell32   ws2_32   dbghelp)
        set(AREG_LDFLAGS_STR    "-lstdc++ -lm -lpthread -ladvapi32 -lpsapi -lshell32 -lws2_32 -ldbghelp")
    endif()
else()

    # POSIX API
    set(AREG_DEVELOP_ENV "Posix")
    add_definitions(-DPOSIX)
    # GNU compile options
    if (CYGWIN)
        list(APPEND AREG_COMPILER_OPTIONS -pthread -Wall -c -fmessage-length=0 -MMD ${AREG_USER_DEFINES})
        set(AREG_COMPILER_VERSION  -std=gnu++17)
    else()
        list(APPEND AREG_COMPILER_OPTIONS -pthread -Wall -c -fmessage-length=0 -MMD ${AREG_USER_DEFINES})
        set(AREG_COMPILER_VERSION -std=c++17)
    endif()

    if(${CMAKE_BUILD_TYPE} MATCHES "Release")
        list(APPEND AREG_COMPILER_OPTIONS -O3 -ffunction-sections -fdata-sections -fvisibility=hidden "$<$<COMPILE_LANGUAGE:CXX>:-fvisibility-inlines-hidden>")
        if (NOT CMAKE_CROSSCOMPILING)
            list(APPEND AREG_COMPILER_OPTIONS -march=native -mtune=native)
        endif()
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
    else()
        list(APPEND AREG_COMPILER_OPTIONS -O0 -g3)
    endif()

    if (${AREG_ARCH} STREQUAL ${_proc_x86} OR ${AREG_ARCH} STREQUAL ${_proc_x64})
        if(${AREG_BITNESS} EQUAL 32)
            list(APPEND AREG_COMPILER_OPTIONS -m32)
            list(APPEND AREG_LDFLAGS -m32)
        else()
            list(APPEND AREG_COMPILER_OPTIONS -m64)
            list(APPEND AREG_LDFLAGS -m64)
        endif()
    endif()

    # Linker flags (-l is not necessary)
    if (AREG_PLATFORM_MACOS)
        if(${CMAKE_BUILD_TYPE} MATCHES "Release")
            list(APPEND AREG_LDFLAGS -Wl,-dead_strip m pthread)
            set(AREG_LDFLAGS_STR "-Wl,-dead_strip -lm -lpthread")
        else()
            list(APPEND AREG_LDFLAGS m pthread)
            set(AREG_LDFLAGS_STR "-lm -lpthread")
        endif()
    else()
        if(${CMAKE_BUILD_TYPE} MATCHES "Release")
            list(APPEND AREG_LDFLAGS -Wl,--gc-sections -Wl,-O1 stdc++ m pthread rt)
            set(AREG_LDFLAGS_STR "-Wl,--gc-sections -Wl,-O1 -lstdc++ -lm -lpthread -lrt")
        else()
            list(APPEND AREG_LDFLAGS stdc++ m pthread rt)
            set(AREG_LDFLAGS_STR "-lstdc++ -lm -lpthread -lrt")
        endif()

        # GCC on x86-64 does not inline 16-byte (128-bit) atomics
        # Detect this at configure time
        include(CheckCXXSourceCompiles)
        check_cxx_source_compiles("
            #include <atomic>
            #include <cstdint>
            struct alignas(16) Align16 { uint64_t a; uint64_t b; };
            std::atomic<Align16> x;
            int main() {
                Align16 expected{0, 0}, desired{1, 1};
                return x.compare_exchange_strong(expected, desired);
            }
        " AREG_HAVE_NATIVE_16BYTE_ATOMICS)

        if(NOT AREG_HAVE_NATIVE_16BYTE_ATOMICS)
            list(APPEND AREG_LDFLAGS atomic)
            string(APPEND AREG_LDFLAGS_STR " -latomic")
        endif()
    endif()
endif()

# disable SQLite warnings
list(APPEND AREG_OPT_DISABLE_WARN_THIRDPARTY
        -w
)

list(APPEND AREG_OPT_DISABLE_WARN_COMMON
        -Wno-psabi
)
