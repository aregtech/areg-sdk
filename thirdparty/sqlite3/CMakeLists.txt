# ###########################################################################
# Either build 'aregsqlite3' static library or use system available 'sqlite3' build
# ###########################################################################


set(AREG_SQLITE_INCLUDES)
set(AREG_SQLITE_LIBRARY)

if (AREG_SQLITE_PACKAGE)
    # search installed SQLite3 package in the system.
    macro_find_sqlite_package(${AREG_PROCESSOR} ${AREG_BITNESS} AREG_SQLITE_INCLUDES AREG_SQLITE_LIBRARY AREG_SQLITE_FOUND)

    if (AREG_SQLITE_FOUND)

        message(STATUS "AREG: >>> Found SQLile pre-compiled library '${AREG_SQLITE_LIBRARY}', include '${AREG_SQLITE_INCLUDES}'")
        # If SQLite3 package found set appropriate preprocessor option to include right header.
        set(AREG_SQLITE_LIB_REF SQLite::SQLite3)
        set(AREG_SQLITE_LIB     sqlite3)
        add_definitions(-DUSE_SQLITE_PACKAGE=1)
    
        if (NOT "${AREG_SQLITE_INCLUDES}" STREQUAL "")
            include_directories(AFTER "${AREG_SQLITE_INCLUDES}")
        endif()

    else(AREG_SQLITE_FOUND)

        message(STATUS "AREG: >>> Could not find SQLile pre-compiled library  Builds 'aregsqlite3' library from sources.")
        # The SQLite3 package did not find, compile from available sources.
        set(AREG_SQLITE_PACKAGE OFF CACHE INTERNAL "Use SQLite3 installed package")
        set(sqlite_BASE "${AREG_THIRDPARTY}/sqlite3")
        list(APPEND sqlite_SRC
            ${sqlite_BASE}/amalgamation/sqlite3.c
        )

        include_directories(${sqlite_BASE})

        set(AREG_SQLITE_LIB_REF ${AREG_PACKAGE_NAME}::aregsqlite3)
        set(AREG_SQLITE_LIB     aregsqlite3)
        addStaticLibEx_C(${AREG_SQLITE_LIB} ${AREG_PACKAGE_NAME} "${sqlite_SRC}" "")
        target_compile_options(${AREG_SQLITE_LIB} PRIVATE -DUSE_SQLITE_PACKAGE=0 "${AREG_OPT_DISABLE_WARN_THIRDPARTY}")

    endif(AREG_SQLITE_FOUND)

else()
    set(AREG_SQLITE_FOUND FALSE)
endif()
