#ifndef AREG_LOGGING_IELOGDATABASEENGINE_HPP
#define AREG_LOGGING_IELOGDATABASEENGINE_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/logging/IELogDatabaseEngine.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, the Database engine to log messages.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/persist/IEDatabaseEngine.hpp"

#include "areg/logging/NELogging.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class DateTime;
namespace NEService {
    struct sServiceConnectedInstance;
}

//////////////////////////////////////////////////////////////////////////
// IELogDatabaseEngine class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The interface to logging database engine used to save messages in the database.
 **/
class AREG_API IELogDatabaseEngine  : public IEDatabaseEngine
{
//////////////////////////////////////////////////////////////////////////
// Constructor / destructor
//////////////////////////////////////////////////////////////////////////
protected:
    IELogDatabaseEngine(void);
    virtual ~IELogDatabaseEngine(void);

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// IELogDatabaseEngine interface overrides.
/************************************************************************/

    /**
     * \brief   Returns true if the database and the log tables are initialized,
     *          and ready to log messages.
     **/
    virtual bool areTablesInitialized(void) const = 0;

    /**
     * \brief   Called when logging message should be saved in the database.
     * \param   message     The structure of the message to log.
     * \return  Returns true if succeeded to save the log in the database.
     **/
    virtual bool logMessage(const NELogging::sLogMessage & message) = 0;

    /**
     * \brief   Called when need to log information about log source instance.
     * \param   instance    The structure of the logging message source to save in database.
     * \param   timestamp   The timestamp to register when the instance is logged.
     * \return  Returns true if succeeded to save the log instance in the database.
     **/
    virtual bool logInstanceConnected(const NEService::sServiceConnectedInstance & instance, const DateTime & timestamp) = 0;

    /**
     * \brief   Called when an instance of log source is disconnected.
     *          This call should as well automatically deactivate the log scopes.
     * \param   cookie      The cookie ID of the instance to mark as disconnected.
     * \param   timestamp   The deactivation timestamp to set.
     * \return  Returns true if operation succeeded.
     **/
    virtual bool logInstanceDisconnected(const ITEM_ID & cookie, const DateTime & timestamp) = 0;

    /**
     * \brief   Called when need to log the information of the scope in the database.
     * \param   scope       The log scope information to save in the database.
     * \param   cookie      The cookie of the log scope owner instance.
     * \param   timestamp   The timestamp to register when the scope is logged.
     * \return  Returns true if succeeded to save the log scope in the database.
     **/
    virtual bool logScopeActivate(const NELogging::sScopeInfo & scope, const ITEM_ID & cookie, const DateTime & timestamp) = 0;

    /**
     * \brief   Called when need to log the information of the scope in the database.
     * \param   scopeName   The name of the scope.
     * \param   scopeId     The ID of the scope.
     * \param   scopePrio   The log priority of the scope
     * \param   cookie      The cookie of the log scope owner instance.
     * \param   timestamp   The timestamp to register when the scope is logged.
     * \return  Returns true if succeeded to save the log scope in the database.
     **/
    virtual bool logScopeActivate(const String & scopeName, uint32_t scopeId, uint32_t scopePrio, const ITEM_ID & cookie, const DateTime & timestamp) = 0;

    /**
     * \brief   Called when need to log the information of the list of scopes in the database.
     * \param   scopes      The information of the list of the log scope to save in the database.
     * \param   cookie      The cookie of the log scope owner instance.
     * \param   timestamp   The timestamp to register when the scope is logged.
     * \return  Returns the number of scope entries saved in the database.
     **/
    virtual uint32_t logScopesActivate(const NELogging::ScopeNames& scopes, const ITEM_ID& cookie, const DateTime& timestamp) = 0;

    /**
     * \brief   Call to deactivate all scopes related with the specified cookie ID.
     * \param   cookie      The cookie ID, which scopes should be marked as deactivated.
     * \param   timestamp   The deactivation timestamp to set.
     * \return  Returns true if operation succeeded.
     **/
    virtual bool logScopesDeactivate(const ITEM_ID & cookie, const DateTime & timestamp) = 0;

    /**
     * \brief   Call to deactivate a single scope related with the specified cookie ID.
     * \param   cookie      The cookie ID, which scope should be marked as deactivated.
     * \param   scopeId     The scope ID to mark as deactivated.
     * \param   timestamp   The deactivation timestamp to set.
     * \return  Returns true if operation succeeded.
     **/
    virtual bool logScopeDeactivate(const ITEM_ID & cookie, unsigned int scopeId, const DateTime & timestamp) = 0;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    DECLARE_NOCOPY_NOMOVE(IELogDatabaseEngine);
};

#endif // AREG_LOGGING_IELOGDATABASEENGINE_HPP
