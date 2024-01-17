set(sqlite_BASE "${AREG_THIRDPARTY}/sqlite3")
list(APPEND sqlite_SRC
    ${sqlite_BASE}/amalgamation/sqlite3.c
)

include_directories(${sqlite_BASE})

addStaticLib(sqlite3 "${sqlite_SRC}")
