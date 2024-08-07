set(sqlite_BASE "${AREG_THIRDPARTY}/sqlite3")
list(APPEND sqlite_SRC
    ${sqlite_BASE}/amalgamation/sqlite3.c
)

include_directories(${sqlite_BASE})

addStaticLib_C(sqlite3 "${sqlite_SRC}")
target_compile_options(sqlite3 PRIVATE "${AREG_OPT_DISABLE_WARN_THIRDPARTY}")
