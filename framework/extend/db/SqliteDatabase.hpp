#ifndef AREG_UTILITIES_DB_SQLITEDATABASE_HPP
#define AREG_UTILITIES_DB_SQLITEDATABASE_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        extend/db/SqliteDatabase.hpp
 * \author      Artak Avetyan
 * \ingroup     AREG platform, extended library, SQLite Database
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/persist/IEDatabaseEngine.hpp"
#include "areg/base/String.hpp"

//////////////////////////////////////////////////////////////////////////
// SqliteDatabase class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The SQLite Database engine.
 **/
class SqliteDatabase  : public IEDatabaseEngine
{
//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor. Creates empty object, not connected to SQLite database.
     **/
    SqliteDatabase(void);

    /**
     * \brief   Sets the SQLite database file path and connects to the database if 'open' flag is true.
     * \param   dbPath  The relative or absolute path to the SQLite database file. The path may have a mask.
     * \param   open    The flag, indicating whether it needs to connect to the database or not.
     **/
    SqliteDatabase(const String & dbPath, bool open);

    /**
     * \brief   Closes SQLite database file if it is still opened.
     **/
    virtual ~SqliteDatabase(void);

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns the path to the SQLite database file.
     **/
    inline const String & getPath(void) const;

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// IEDatabaseEngine interface overrides.
/************************************************************************/

    /**
     * \brief   Returns true if SqliteDatabase engine is opened and operable.
     *          Otherwise, returns false.
     **/
    virtual bool isOperable(void) const override;

    /**
     * \brief   Connects to the specified SqliteDatabase.
     * \param   dbPath  The path to the SqliteDatabase. If needed, the path may contain
     *                  file path or URL, user name and password. It is up to
     *                  SqliteDatabase engine to parse the path and initialize the connection.
     *                  If the parameter is empty, it should take the data from the
     *                  'areg.init' configuration file.
     * \return  Returns true if succeeded to connect. Otherwise, returns false.
     **/
    virtual bool connect(const String & dbPath) override;

    /**
     * \brief   Disconnects connected SqliteDatabase.
     **/
    virtual void disconnect(void) override;

    /**
     * \brief   Execute the SQL script.
     * \param   sql     The SQL script to execute.
     * \return  Returns true if succeeds to execute the SQL script.
     **/
    virtual bool execute(const String & sql) override;

    /**
     * \brief   Call if need to make multiple operation. This call starts the transaction,
     *          that is required either commit or rollback call to complete the transaction.
     **/
    virtual bool begin(void) override;

    /**
     * \brief   Commits or rolls back the SqliteDatabase changes and returns true if succeeded.
     * \param   doCommit    If true, the SqliteDatabase engine should commit the changes.
     *                      Otherwise, the SqliteDatabase engine should rollback the changes.
     * \return  Returns true if operation succeeded. Otherwise, returns false.
     **/
    virtual bool commit(bool doCommit) override;

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   Opens or creates the specified database file.
     * \param   dbPath      The relative or absolute path the database file.
     *                      The file path may contain a mask.
     * \return  Returns true if succeeded to open or create the database file.
     **/
    inline bool _open(const String& dbPath);

    /**
     * \brief   Closes previously opened database and releases resources.
     **/
    inline void _close(void);

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
    DECLARE_NOCOPY_NOMOVE(SqliteDatabase);
};

//////////////////////////////////////////////////////////////////////////
// SqliteDatabase class inline methods
//////////////////////////////////////////////////////////////////////////

inline const String& SqliteDatabase::getPath(void) const
{
    return mDbPath;
}

#endif // AREG_UTILITIES_DB_SQLITEDATABASE_HPP
