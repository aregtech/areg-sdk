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
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        aregextend/db/SqliteDatabase.hpp
 * \author      Artak Avetyan
 * \ingroup     Areg platform, extended library, SQLite Database
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/persist/DatabaseEngine.hpp"
#include "areg/base/String.hpp"

namespace areg::ext {

//////////////////////////////////////////////////////////////////////////
// SqliteDatabase class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   SQLite database engine with connection management, transaction support, and query
 *          execution.
 **/
class SqliteDatabase final : public DatabaseEngine
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

    virtual ~SqliteDatabase();

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:

    /**
      * \brief   Returns the path to the SQLite database file.
      **/
    [[nodiscard]]
    inline const String & path() const noexcept;

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// DatabaseEngine class overrides.
/************************************************************************/

    /**
     * \brief   Returns true if the SQLite database engine is opened and ready for operations; false otherwise.
     **/
    [[nodiscard]]
    bool is_operable() const noexcept final;

    /**
     * \brief   Connects to the specified SQLite database.
     *
     * \param   dbPath      The path to the database. May be a file path, URL, or contain
     *                      authentication. If empty, uses the path from the 'areg.init' configuration file.
     * \param   readOnly    If true, connects in read-only mode; if false, connects in read-write mode.
     * \return  True if the connection succeeded; false otherwise.
     **/
    bool connect(const String& dbPath, bool readOnly) final;

    /**
     * \brief   Disconnects from the database and releases resources.
     **/
    void disconnect() final;

    /**
     * \brief   Executes the SQL script and returns the result.
     *
     * \param   sql     The SQL script to execute.
     * \return  True if the SQL script executed successfully; false otherwise.
     **/
    bool execute(const String & sql) final;

    /**
     * \brief   Begins a transaction. Must be followed by commit() or rollback() to complete the
     *          transaction.
     **/
    bool begin() final;

    /**
     * \brief   Commits or rolls back the current transaction.
     *
     * \param   doCommit    If true, commits the transaction; if false, rolls back the changes.
     * \return  True if the operation succeeded; false otherwise.
     **/
    bool commit(bool doCommit) final;

    /**
     * \brief   Rolls back the current transaction and returns true if successful; false otherwise.
     **/
    bool rollback() final;

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
    inline void _close() noexcept;

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

inline const String& SqliteDatabase::path() const noexcept
{
    return mDbPath;
}

} // namespace areg::ext

#endif // AREG_AREGEXTEND_DB_SQLITEDATABASE_HPP
