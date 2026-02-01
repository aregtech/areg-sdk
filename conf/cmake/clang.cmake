# ###########################################################################
# LLVM compiler and linker options for POSIX and Windows API
# Copyright 2022-2026 Aregtech
# ###########################################################################

message(STATUS "Areg: >>> Preparing settings for CLang compiler under \'${AREG_OS}\' platform, WIN32 = \'${WIN32}\'")

if (AREG_PLATFORM_WINDOWS)

    if(CMAKE_BUILD_TYPE MATCHES Release)
        list(APPEND AREG_COMPILER_OPTIONS -O2)
    else()
        list(APPEND AREG_COMPILER_OPTIONS -Od -RTC1 -c)
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

    if(NOT CMAKE_BUILD_TYPE MATCHES Release)
        list(APPEND AREG_COMPILER_OPTIONS -O0 -g3)
    else()
        list(APPEND AREG_COMPILER_OPTIONS -O2)
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
        list(APPEND AREG_LDFLAGS stdc++   m   pthread)
        set(AREG_LDFLAGS_STR  "-lstdc++ -lm -lpthread")
        set(AREG_COMPILER_VERSION -stdlib=libc++)
    else()
        list(APPEND AREG_LDFLAGS stdc++   m   pthread   rt)
        set(AREG_LDFLAGS_STR  "-lstdc++ -lm -lpthread -lrt")
        set(AREG_COMPILER_VERSION -stdlib=libstdc++)
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
