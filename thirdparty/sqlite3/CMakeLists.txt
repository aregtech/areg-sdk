# ###########################################################################
# Either build 'sqlite3' static library or use system available build
# ###########################################################################


set(AREG_SQLITE_INCLUDES)
set(AREG_SQLITE_LIBRARY)

if (AREG_SQLITE_PACKAGE)
    macro_find_package(SQLite3 AREG_FOUND_SQLITE3_PACKAGE AREG_SQLITE_INCLUDES AREG_SQLITE_LIBRARY)
endif()

if (AREG_FOUND_SQLITE3_PACKAGE)

    set(AREG_SQLITE_LIB_REF SQLite::SQLite3)
    include_directories(AFTER "${AREG_SQLITE_INCLUDES}")
    add_definitions(-DUSE_SQLITE_PACKAGE=1)
    
else(AREG_FOUND_SQLITE3_PACKAGE)

    set(sqlite_BASE "${AREG_THIRDPARTY}/sqlite3")
    list(APPEND sqlite_SRC
        ${sqlite_BASE}/amalgamation/sqlite3.c
    )

    include_directories(${sqlite_BASE})

    set(AREG_SQLITE_LIB_REF sqlite3)
    addStaticLib_C(${AREG_SQLITE_LIB_REF} "${sqlite_SRC}")
    target_compile_options(${AREG_SQLITE_LIB_REF} PRIVATE -DUSE_SQLITE_PACKAGE=0 "${AREG_OPT_DISABLE_WARN_THIRDPARTY}")

endif()
