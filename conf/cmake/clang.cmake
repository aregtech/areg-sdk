# ###########################################################################
# LLVM compiler and linker options for POSIX and Windows API
# Copyright 2022-2026 Aregtech (Artak Avetyan)
# ###########################################################################

message(STATUS "Areg: >>> Preparing settings for CLang compiler under \'${AREG_OS}\' platform, WIN32 = \'${WIN32}\'")

if (AREG_PLATFORM_WINDOWS)

    get_property(_areg_multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
    if (_areg_multi_config)
        # Multi-config generator (Visual Studio + ClangCL): scope flags per-configuration.
        list(APPEND AREG_COMPILER_OPTIONS
            $<$<CONFIG:Release>:-O3>
            $<$<CONFIG:Release>:-ffunction-sections>
            $<$<CONFIG:Release>:-fdata-sections>
            $<$<NOT:$<CONFIG:Release>>:-Od>
            $<$<NOT:$<CONFIG:Release>>:-RTC1>
        )
        if (NOT CMAKE_CROSSCOMPILING)
            if (AREG_ARCH_NATIVE)
                list(APPEND AREG_COMPILER_OPTIONS
                    $<$<CONFIG:Release>:-march=native>
                    $<$<CONFIG:Release>:-mtune=native>
                )
            elseif (${AREG_ARCH} STREQUAL ${_proc_x86} OR ${AREG_ARCH} STREQUAL ${_proc_x64})
                # Portable SSE4.2 baseline, keeps hardware CRC32C (MathDefs.hpp).
                macro_check_sse42(AREG_SSE42_HW)
                if (AREG_SSE42_HW)
                    list(APPEND AREG_COMPILER_OPTIONS $<$<CONFIG:Release>:-msse4.2>)
                endif()
            endif()
        endif()
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
    else()
        # Single-config generator: CMAKE_BUILD_TYPE is reliable.
        if(${CMAKE_BUILD_TYPE} MATCHES "Release")
            list(APPEND AREG_COMPILER_OPTIONS -O3 -ffunction-sections -fdata-sections)
            if (NOT CMAKE_CROSSCOMPILING)
                if (AREG_ARCH_NATIVE)
                    list(APPEND AREG_COMPILER_OPTIONS -march=native -mtune=native)
                elseif (${AREG_ARCH} STREQUAL ${_proc_x86} OR ${AREG_ARCH} STREQUAL ${_proc_x64})
                    # Portable SSE4.2 baseline, keeps hardware CRC32C (MathDefs.hpp).
                    macro_check_sse42(AREG_SSE42_HW)
                    if (AREG_SSE42_HW)
                        list(APPEND AREG_COMPILER_OPTIONS -msse4.2)
                    endif()
                endif()
            endif()
            set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
        else()
            list(APPEND AREG_COMPILER_OPTIONS -Od -RTC1 -c)
        endif()
    endif()

    # Win32 API
    set(AREG_DEVELOP_ENV "Win32")
    add_definitions(-DWINDOWS -D_WINDOWS -DWIN32 -D_WIN32)
    if (${AREG_BITNESS} EQUAL 64)
        add_definitions(-DWIN64 -D_WIN64)
    endif()

    # Clang compile options
    list(APPEND AREG_COMPILER_OPTIONS -Wall -c)
    # Linker flags (-l is not necessary)
    list(APPEND AREG_LDFLAGS  advapi32   psapi   shell32   ws2_32)
    set(AREG_LDFLAGS_STR   "-ladvapi32 -lpsapi -lshell32 -lws2_32")

else()

    if(${CMAKE_BUILD_TYPE} MATCHES "Release")
        list(APPEND AREG_COMPILER_OPTIONS -O3 -ffunction-sections -fdata-sections -fvisibility=hidden "$<$<COMPILE_LANGUAGE:CXX>:-fvisibility-inlines-hidden>")
        if (NOT CMAKE_CROSSCOMPILING)
            if (AREG_ARCH_NATIVE)
                list(APPEND AREG_COMPILER_OPTIONS -march=native -mtune=native)
            elseif (${AREG_ARCH} STREQUAL ${_proc_x86} OR ${AREG_ARCH} STREQUAL ${_proc_x64})
                # Portable SSE4.2 baseline, keeps hardware CRC32C (MathDefs.hpp).
                macro_check_sse42(AREG_SSE42_HW)
                if (AREG_SSE42_HW)
                    list(APPEND AREG_COMPILER_OPTIONS -msse4.2)
                endif()
            endif()
        endif()
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
    else()
        list(APPEND AREG_COMPILER_OPTIONS -O0 -g3)
    endif()

    # POSIX API
    set(AREG_DEVELOP_ENV "Posix")
    add_definitions(-DPOSIX)

    if(${AREG_BITNESS} EQUAL 32)
        list(APPEND AREG_COMPILER_OPTIONS -m32)
        list(APPEND AREG_LDFLAGS -m32)
    else()
        list(APPEND AREG_COMPILER_OPTIONS -m64)
        list(APPEND AREG_LDFLAGS -m64)
    endif()

    # Clang compile options
    list(APPEND AREG_COMPILER_OPTIONS -pthread -Wall -c -fmessage-length=0)
    # Linker flags (-l is not necessary)
    if (AREG_PLATFORM_MACOS)
        if(${CMAKE_BUILD_TYPE} MATCHES "Release")
            list(APPEND AREG_LDFLAGS -Wl,-dead_strip m pthread)
            set(AREG_LDFLAGS_STR  "-Wl,-dead_strip -lm -lpthread")
        else()
            list(APPEND AREG_LDFLAGS m   pthread)
            set(AREG_LDFLAGS_STR  "-lm -lpthread")
        endif()
        if(NOT ("c++" IN_LIST CMAKE_CXX_IMPLICIT_LINK_LIBRARIES))
            list(APPEND AREG_LDFLAGS c++)
            string(APPEND AREG_LDFLAGS_STR " -lc++")
        endif()
        set(AREG_COMPILER_VERSION -stdlib=libc++)
    else()
        if(${CMAKE_BUILD_TYPE} MATCHES "Release")
            list(APPEND AREG_LDFLAGS -Wl,--gc-sections -Wl,-O1 stdc++ m pthread rt)
            set(AREG_LDFLAGS_STR  "-Wl,--gc-sections -Wl,-O1 -lstdc++ -lm -lpthread -lrt")
        else()
            list(APPEND AREG_LDFLAGS stdc++   m   pthread   rt)
            set(AREG_LDFLAGS_STR  "-lstdc++ -lm -lpthread -lrt")
        endif()
        set(AREG_COMPILER_VERSION -stdlib=libstdc++)

        # Clang on non-x86-64 Linux (e.g. ARMv7, RISC-V) may emit __atomic_*_16
        # Check passes, libatomic is not pulled in for the common case.
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

# Disable clang common warnings
list(APPEND AREG_OPT_DISABLE_WARN_COMMON
        -Wno-c++98-compat
        -Wno-c++98-compat-pedantic
        -Wno-covered-switch-default
        -Wno-exit-time-destructors
        -Wno-extra-semi-stmt
        -Wno-float-equal
        -Wno-global-constructors
        -Wno-inconsistent-missing-destructor-override
        -Wno-language-extension-token
        -Wno-missing-prototypes
        -Wno-old-style-cast
        -Wno-reserved-identifier
        -Wno-reserved-macro-identifier
        -Wno-suggest-destructor-override
        -Wno-undefined-func-template
        -Wno-unknown-warning-option
        -Wno-unsafe-buffer-usage
)

# disable framework warnings
list(APPEND AREG_OPT_DISABLE_WARN_FRAMEWORK
        -Wno-cast-calling-convention
        -Wno-cast-function-type
        -Wno-format-nonliteral
        -Wno-null-pointer-subtraction
)

# disable tools warnings
list(APPEND AREG_OPT_DISABLE_WARN_TOOLS
        -Wno-format-nonliteral
        -Wno-unused-parameter
)

# disable example warnings (example 17 with MFC)
list(APPEND AREG_OPT_DISABLE_WARN_EXAMPLES
        -Wno-cast-function-type 
        -Wno-cast-qual
        -Wno-disabled-macro-expansion
        -Wno-extra-semi-stmt
        -Wno-keyword-macro
        -Wno-sign-conversion 
        -Wno-unused-local-typedef
        -Wno-unused-macros
        -Wno-unused-parameter
)

# disable warnings for generated codes
list(APPEND AREG_OPT_DISABLE_WARN_CODEGEN 
        -Wno-switch-enum
        -Wno-extra-semi-stmt
        -Wno-unused-parameter
)

# disable SQLite warnings
list(APPEND AREG_OPT_DISABLE_WARN_THIRDPARTY
        -Wno-everything
)
