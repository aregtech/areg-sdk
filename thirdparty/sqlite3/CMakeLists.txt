# ###########################################################################
# Either build 'sqlite3' static library or use system available build
# ###########################################################################


set(AREG_SQLITE_INCLUDES)
set(AREG_SQLITE_LIBRARY)

if (AREG_SQLITE_PACKAGE)
    # search installed SQLite3 package in the system.
    macro_find_package(SQLite3 AREG_SQLITE_FOUND AREG_SQLITE_INCLUDES AREG_SQLITE_LIBRARY)
endif()

if (AREG_SQLITE_FOUND)

    message(STATUS "AREG: >>> Will use existing sqlite3 package")
    # If SQLite3 package found set appropriate preprocessor option to include right header.
    set(AREG_SQLITE_LIB_REF SQLite::SQLite3)
    add_definitions(-DUSE_SQLITE_PACKAGE=1)
    
    if (NOT "${AREG_SQLITE_INCLUDES}" STREQUAL "")
        include_directories(AFTER "${AREG_SQLITE_INCLUDES}")
    endif()

else(AREG_SQLITE_FOUND)

    message(STATUS "AREG: >>> Will compile sqlite3 library")
    # The SQLite3 package did not find, compile from available sources.
    set(AREG_SQLITE_PACKAGE OFF CACHE BOOL "Use SQLite3 installed package" FORCE)
    set(sqlite_BASE "${AREG_THIRDPARTY}/sqlite3")
    list(APPEND sqlite_SRC
        ${sqlite_BASE}/amalgamation/sqlite3.c
    )

    include_directories(${sqlite_BASE})

    set(AREG_SQLITE_LIB_REF sqlite3)
    addStaticLib_C(${AREG_SQLITE_LIB_REF} "${sqlite_SRC}")
    target_compile_options(${AREG_SQLITE_LIB_REF} PRIVATE -DUSE_SQLITE_PACKAGE=0 "${AREG_OPT_DISABLE_WARN_THIRDPARTY}")

endif(AREG_SQLITE_FOUND)
