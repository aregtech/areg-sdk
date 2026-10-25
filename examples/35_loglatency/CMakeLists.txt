macro_declare_executable(35_logsource logsource/src/main.cpp)

if (AREG_LOGGER_LIB_TYPE MATCHES "shared")
    macro_declare_executable(35_logsink logsink/src/main.cpp areglogger)
    target_compile_definitions(35_logsink PRIVATE IMP_LOGGER_DLL)
else ()
    macro_declare_executable(35_logsink logsink/src/main.cpp areglogger ${AREG_SQLITE_LIB_REF})
    target_compile_definitions(35_logsink PRIVATE IMP_LOGGER_LIB)
endif()
