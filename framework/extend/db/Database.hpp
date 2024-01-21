#ifndef AREG_UTILITIES_DB_DATABASE_HPP
#define AREG_UTILITIES_DB_DATABASE_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        extend/db/Database.hpp
 * \author      Artak Avetyan
 * \ingroup     AREG platform, extended library, SQLite database
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/persist/IEDatabaseEngine.hpp"
#include "sqlite3/amalgamation/sqlite3.h"

#include "areg/base/String.hpp"

/**
 * \brief   The database engine
 **/
class Database  : public IEDatabaseEngine
{
public:
    Database(void);
    Database(const String& dbPath, bool open);
    virtual ~Database(void);

public:

    inline const String& getPath(void) const;

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// IEDatabaseEngine interface overrides.
/************************************************************************/

    /**
     * \brief   Returns true if database engine is opened and operable.
     *          Otherwise, returns false.
     **/
    virtual bool isOperable(void) const override;

    /**
     * \brief   Connects to the specified database.
     * \param   dbpath  The path to the database. If needed, the path may contain
     *                  file path or URL, user name and password. It is up to
     *                  Database engine to parse the path and initialize the connection.
     * \return  Returns true if succeeded to connect. Otherwise, returns false.
     **/
    virtual bool connect(const String& dbPath) override;

    /**
     * \brief   Disconnects connected database.
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
     * \brief   Commits or rolls back the database changes and returns true if succeeded.
     * \param   doCommit    If true, the database engine should commit the changes.
     *                      Otherwise, the database engine should rollback the changes.
     * \return  Returns true if operation succeeded. Otherwise, returns false.
     **/
    virtual bool commit(bool doCommit) override;

private:

    inline bool _open(const String& dbPath);

    inline void _close(void);

protected:
    String      mDbPath;
    sqlite3*    mDbObject;

private:
    DECLARE_NOCOPY_NOMOVE(Database);
};

inline const String& Database::getPath(void) const
{
    return mDbPath;
}


#endif // AREG_UTILITIES_DB_DATABASE_HPP
