#ifndef AREG_PERSIST_IEDATABASEENGINE_HPP
#define AREG_PERSIST_IEDATABASEENGINE_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/persist/IEDatabaseEngine.hpp
 * \author      Artak Avetyan
 * \ingroup     AREG platform, the interface to the database engine.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/String.hpp"

//////////////////////////////////////////////////////////////////////////
// IEDatabaseEngine class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Interface of database engine.
 **/
class AREG_API IEDatabaseEngine
{
//////////////////////////////////////////////////////////////////////////
// Constructor / destructor
//////////////////////////////////////////////////////////////////////////
protected:
    IEDatabaseEngine(void);

    virtual ~IEDatabaseEngine(void);

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
    virtual bool isOperable(void) const = 0;

    /**
     * \brief   Connects to the specified database.
     * \param   dbPath  The path to the database. If needed, the path may contain
     *                  file path or URL, user name and password. It is up to
     *                  Database engine to parse the path and initialize the connection.
     *                  If the parameter is empty, it should take the data from the
     *                  'areg.init' configuration file.
     * \return  Returns true if succeeded to connect. Otherwise, returns false.
     **/
    virtual bool connect(const String & dbPath) = 0;

    /**
     * \brief   Disconnects connected database.
     **/
    virtual void disconnect(void) = 0;

    /**
     * \brief   Execute the SQL script.
     * \param   sql     The SQL script to execute.
     * \return  Returns true if succeeds to execute the SQL script.
     **/
    virtual bool execute(const String & sql) = 0;

    /**
     * \brief   Call if need to make multiple operation. This call starts the transaction,
     *          that is required either commit or rollback call to complete the transaction.
     **/
    virtual bool begin(void) = 0;

    /**
     * \brief   Commits or rolls back the database changes and returns true if succeeded.
     * \param   doCommit    If true, the database engine should commit the changes.
     *                      Otherwise, the database engine should rollback the changes.
     * \return  Returns true if operation succeeded. Otherwise, returns false.
     **/
    virtual bool commit(bool doCommit) = 0;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    DECLARE_NOCOPY_NOMOVE(IEDatabaseEngine);
};

#endif  // AREG_PERSIST_IEDATABASEENGINE_HPP
