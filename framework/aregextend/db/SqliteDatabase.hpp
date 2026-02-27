#ifndef AREG_AREGEXTEND_DB_SQLITEDATABASE_HPP
#define AREG_AREGEXTEND_DB_SQLITEDATABASE_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        aregextend/db/SqliteDatabase.hpp
 * \author      Artak Avetyan
 * \ingroup     Areg platform, extended library, SQLite Database
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/persist/DatabaseEngine.hpp"
#include "areg/base/String.hpp"

//////////////////////////////////////////////////////////////////////////
// SqliteDatabase class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   SQLite database engine with connection management, transaction support, and query
 *          execution.
 **/
class SqliteDatabase  : public DatabaseEngine
{
    friend class SqliteStatement;
//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    SqliteDatabase();

    /**
     * \brief   Sets the SQLite database file path and connects to the database if 'open' flag is
     *          true.
     *
     * \param   dbPath      The relative or absolute path to the SQLite database file. The path may
     *                      have a mask.
     * \param   open        If true, connects to the database; if false, only sets the path.
     **/
    SqliteDatabase(const String & dbPath, bool open);

    /**
     * \brief   Closes SQLite database file if it is still opened.
     **/
    virtual ~SqliteDatabase();

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:

    inline const String & path() const;

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// DatabaseEngine class overrides.
/************************************************************************/

    /**
     * \brief   Returns true if the SQLite database engine is opened and ready for operations; false
     *          otherwise.
     **/
    bool is_operable() const override;

    /**
     * \brief   Connects to the specified SQLite database.
     *
     * \param   dbPath      The path to the database. May be a file path, URL, or contain
     *                      authentication. If empty, uses the path from the 'areg.init'
     *                      configuration file.
     * \param   readOnly    If true, connects in read-only mode; if false, connects in read-write
     *                      mode.
     * \return  True if the connection succeeded; false otherwise.
     **/
    bool connect(const String& dbPath, bool readOnly) override;

    /**
     * \brief   Disconnects from the database and releases resources.
     **/
    void disconnect() override;

    /**
     * \brief   Executes the SQL script and returns the result.
     *
     * \param   sql     The SQL script to execute.
     * \return  True if the SQL script executed successfully; false otherwise.
     **/
    bool execute(const String & sql) override;

    /**
     * \brief   Begins a transaction. Must be followed by commit() or rollback() to complete the
     *          transaction.
     **/
    bool begin() override;

    /**
     * \brief   Commits or rolls back the current transaction.
     *
     * \param   doCommit    If true, commits the transaction; if false, rolls back the changes.
     * \return  True if the operation succeeded; false otherwise.
     **/
    bool commit(bool doCommit) override;

    /**
     * \brief   Rolls back the current transaction and returns true if successful; false otherwise.
     **/
    bool rollback() override;

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   Opens or creates the specified database file.
     *
     * \param   dbPath      The relative or absolute path to the database file. The path may contain
     *                      a mask.
     * \return  True if the database file was opened or created successfully; false otherwise.
     **/
    inline bool _open(const String& dbPath);

    /**
     * \brief   Closes the database connection and releases resources.
     **/
    inline void _close();

//////////////////////////////////////////////////////////////////////////
// Member variables.
//////////////////////////////////////////////////////////////////////////
protected:
    //!< The path to the SQLite database file.
    String      mDbPath;

    //!< The SQLite database object.
    void *      mDbObject;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE(SqliteDatabase);
};

//////////////////////////////////////////////////////////////////////////
// SqliteDatabase class inline methods
//////////////////////////////////////////////////////////////////////////

inline const String& SqliteDatabase::path() const
{
    return mDbPath;
}

#endif // AREG_AREGEXTEND_DB_SQLITEDATABASE_HPP
