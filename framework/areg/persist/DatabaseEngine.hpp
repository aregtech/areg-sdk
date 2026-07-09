#ifndef AREG_PERSIST_DATABASEENGINE_HPP
#define AREG_PERSIST_DATABASEENGINE_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        areg/persist/DatabaseEngine.hpp
 * \author      Artak Avetyan
 * \ingroup     Areg platform, the interface to the database engine.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/String.hpp"
namespace areg {

//////////////////////////////////////////////////////////////////////////
// DatabaseEngine class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Interface of database engine.
 **/
class AREG_API DatabaseEngine
{
//////////////////////////////////////////////////////////////////////////
// Constructor / destructor
//////////////////////////////////////////////////////////////////////////
protected:
    DatabaseEngine() = default;

    virtual ~DatabaseEngine() = default;

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// DatabaseEngine interface overrides.
/************************************************************************/

    /**
     * \brief   Returns true if the database engine is operable.
     **/
    [[nodiscard]]
    virtual bool is_operable() const noexcept = 0;

    /**
     * \brief   Connects to the database at the specified path.
     *
     * \param   dbPath      The path to the database. The path may contain a file path, URL, user
     *                      name, and password. If empty, the engine reads the connection details
     *                      from the 'areg.init' configuration file.
     * \param   readOnly    If true, the database engine connects in read-only mode; otherwise, in
     *                      read-write mode.
     * \return  Returns true if the connection succeeds; false otherwise.
     **/
    virtual bool connect(const String & dbPath, bool readOnly) = 0;

    /**
     * \brief   Disconnects from the database.
     **/
    virtual void disconnect() = 0;

    /**
     * \brief   Executes the specified SQL script.
     *
     * \param   sql     The SQL script to execute.
     * \return  Returns true if the SQL script executes successfully; false otherwise.
     **/
    virtual bool execute(const String & sql) = 0;

    /**
     * \brief   Starts a database transaction.
     *
     * \return  Returns true if the transaction starts successfully; false otherwise.
     **/
    virtual bool begin() = 0;

    /**
     * \brief   Commits or rolls back pending database changes.
     *
     * \param   doCommit    If true, commits the changes; if false, rolls them back.
     * \return  Returns true if the operation succeeds; false otherwise.
     **/
    virtual bool commit(bool doCommit) = 0;

    /**
     * \brief   Rolls back all pending database changes.
     *
     * \return  Returns true if the rollback succeeds; false otherwise.
     **/
    virtual bool rollback() = 0;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE(DatabaseEngine);
};

} // namespace areg
#endif  // AREG_PERSIST_DATABASEENGINE_HPP
