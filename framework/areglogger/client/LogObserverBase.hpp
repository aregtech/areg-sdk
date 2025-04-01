#ifndef AREG_AREGLOGGER_CLIENT_LOGOBSERVERBASE_HPP
#define AREG_AREGLOGGER_CLIENT_LOGOBSERVERBASE_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areglogger/client/LogObserverBase.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, Log Observer library.
 *              The log observer base class.
 ************************************************************************/

 /************************************************************************
  * Include files.
  ************************************************************************/
#include "areglogger/client/LogObserverSwitches.h"
#include "areglogger/client/LogObserverApi.h"

class LOGGER_API LogObserverBase
{
protected:
    LogObserverBase(void);

public:
    virtual ~LogObserverBase(void);

public:

protected:

    /**
     * \brief   The callback of the event triggered when initializing and configuring the observer.
     *          The callback indicates the IP address and port number of the log collector service set
     *          in the configuration file.
     * \param   isEnabled       The flag, indicating whether the logging service is enabled or not.
     * \param   address         The null-terminated string of the IP address of the log collector service set in the configuration file.
     * \param   port            The IP port number of the log collector service set in the configuration file.
     **/
    virtual void onLogObserverConfigured(bool isEnabled, const char* address, uint16_t port) = 0;

    /**
     * \brief   The callback of the event triggered when initializing and configuring the observer.
     *          The callback indicates the supported database, the database location or URI and
     *          the database user name.
     * \param   isEnabled       The flag, indicating whether the logging in the database is enabler or not.
     * \param   dbName          The name of the  supported database.
     * \param   dbLocation      The relative or absolute path the database. The path may contain a mask.
     * \param   dbUser          The database user to use when log in. If null or empty, the database may not require the user name.
     **/
    virtual void onLogLogDbConfigured(bool isEnabled, const char* dbName, const char* dbLocation, const char* dbUser) = 0;

    /**
     * \brief   The callback of the event triggered when the observer connects or disconnects from the log collector service.
     * \param   isConnected     Flag, indicating whether observer is connected or disconnected.
     * \param   address         The IP address of the log collector service to connect or disconnect.
     * \param   port            The IP port number of the log collector service to connect or disconnect.
     **/
    virtual void onLogServiceConnected(bool isConnected, const char* address, uint16_t port) = 0;

    /**
     * \brief   The callback of the event trigger when starting or pausing the log observer.
     *          If the log observer is paused, on start it continues to write logs in the same file.
     *          If the log observer is stopped (disconnected is called), on start it creates new file.
     * \param   isStarted       The flag indicating whether the lob observer is started or paused.
     **/
    virtual void onLogObserverStarted(bool isStarted) = 0;

    /**
     * \brief   The callback of the event triggered when the logging database is created.
     * \param   dbLocation      The relative or absolute path to the logging database.
     **/
    virtual void onLogLogDbCreated(const char* dbLocation) = 0;

    /**
     * \brief   The callback of the event triggered when fails to send or receive message.
     **/
    virtual void onLogMessagingFailed(void) = 0;

    /**
     * \brief   The callback of the event triggered when receive the list of connected instances that make logs.
     * \param   instances   The pointer to the list of the connected instances.
     * \param   count       The number of entries in the list.
     **/
    virtual void onLogInstancesConnect(const sLogInstance* instances, uint32_t count) = 0;

    /**
     * \brief   The callback of the event triggered when receive the list of disconnected instances that make logs.
     * \param   instances   The pointer to the list of IDs of the disconnected instances.
     * \param   count       The number of entries in the list.
     **/
    virtual void onLogInstancesDisconnect(const ITEM_ID* instances, uint32_t count) = 0;

    /**
     * \brief   The callback of the event triggered when receive the list of the scopes registered in an application.
     * \param   cookie  The cookie ID of the connected instance / application. Same as sLogInstance::liCookie
     * \param   scopes  The list of the scopes registered in the application. Each entry contains the ID of the scope, message priority and the full name.
     * \param   count   The number of scope entries in the list.
     **/
    virtual void onLogLogRegisterScopes(ITEM_ID cookie, const sLogScope* scopes, uint32_t count) = 0;

    /**
     * \brief   The callback of the event triggered when receive the list of previously registered scopes with new priorities.
     * \param   cookie  The cookie ID of the connected instance / application. Same as sLogInstance::liCookie
     * \param   scopes  The list of previously registered scopes. Each entry contains the ID of the scope, message priority and the full name.
     * \param   count   The number of scope entries in the list.
     **/
    virtual void onLogLogUpdateScopes(ITEM_ID cookie, const sLogScope* scopes, uint32_t count) = 0;

    /**
     * \brief   The callback of the event triggered when receive message to log.
     * \param   logMessage  The structure of the message to log.
     **/
    virtual void onLogLogMessage(const sLogMessage* logMessage) = 0;

    /**
     * \brief   The callback of the event triggered when receive remote message to log.
     *          The buffer indicates to the NELogging::sLogMessage structure.
     * \param   logBuffer   The pointer to the NELogging::sLogMessage structure to log messages.
     * \param   size        The size of the buffer with log message.
     **/
    virtual void onLogLogMessageEx(const unsigned char* logBuffer, uint32_t size) = 0;

private:
    /**
     * \brief   The callback of the event triggered when initializing and configuring the observer.
     *          The callback indicates the IP address and port number of the log collector service set
     *          in the configuration file.
     * \param   isEnabled       The flag, indicating whether the logging service is enabled or not.
     * \param   address         The null-terminated string of the IP address of the log collector service set in the configuration file.
     * \param   port            The IP port number of the log collector service set in the configuration file.
     **/
    static void _callbackLogObserverConfigured(bool isEnabled, const char* address, uint16_t port);

    /**
     * \brief   The callback of the event triggered when initializing and configuring the observer.
     *          The callback indicates the supported database, the database location or URI and
     *          the database user name.
     * \param   isEnabled       The flag, indicating whether the logging in the database is enabler or not.
     * \param   dbName          The name of the  supported database.
     * \param   dbLocation      The relative or absolute path the database. The path may contain a mask.
     * \param   dbUser          The database user to use when log in. If null or empty, the database may not require the user name.
     **/
    static void _callbackLogLogDbConfigured(bool isEnabled, const char* dbName, const char* dbLocation, const char* dbUser);

    /**
     * \brief   The callback of the event triggered when the observer connects or disconnects from the log collector service.
     * \param   isConnected     Flag, indicating whether observer is connected or disconnected.
     * \param   address         The IP address of the log collector service to connect or disconnect.
     * \param   port            The IP port number of the log collector service to connect or disconnect.
     **/
    static void _callbackLogServiceConnected(bool isConnected, const char* address, uint16_t port);

    /**
     * \brief   The callback of the event trigger when starting or pausing the log observer.
     *          If the log observer is paused, on start it continues to write logs in the same file.
     *          If the log observer is stopped (disconnected is called), on start it creates new file.
     * \param   isStarted       The flag indicating whether the lob observer is started or paused.
     **/
    static void _callbackLogObserverStarted(bool isStarted);

    /**
     * \brief   The callback of the event triggered when the logging database is created.
     * \param   dbLocation      The relative or absolute path to the logging database.
     **/
    static void _callbackLogLogDbCreated(const char* dbLocation);

    /**
     * \brief   The callback of the event triggered when fails to send or receive message.
     **/
    static void _callbackLogMessagingFailed(void);

    /**
     * \brief   The callback of the event triggered when receive the list of connected instances that make logs.
     * \param   instances   The pointer to the list of the connected instances.
     * \param   count       The number of entries in the list.
     **/
    static void _callbackLogInstancesConnect(const sLogInstance* instances, uint32_t count);

    /**
     * \brief   The callback of the event triggered when receive the list of disconnected instances that make logs.
     * \param   instances   The pointer to the list of IDs of the disconnected instances.
     * \param   count       The number of entries in the list.
     **/
    static void _callbackLogInstancesDisconnect(const ITEM_ID* instances, uint32_t count);

    /**
     * \brief   The callback of the event triggered when receive the list of the scopes registered in an application.
     * \param   cookie  The cookie ID of the connected instance / application. Same as sLogInstance::liCookie
     * \param   scopes  The list of the scopes registered in the application. Each entry contains the ID of the scope, message priority and the full name.
     * \param   count   The number of scope entries in the list.
     **/
    static void _callbackLogLogRegisterScopes(ITEM_ID cookie, const sLogScope* scopes, uint32_t count);

    /**
     * \brief   The callback of the event triggered when receive the list of previously registered scopes with new priorities.
     * \param   cookie  The cookie ID of the connected instance / application. Same as sLogInstance::liCookie
     * \param   scopes  The list of previously registered scopes. Each entry contains the ID of the scope, message priority and the full name.
     * \param   count   The number of scope entries in the list.
     **/
    static void _callbackLogLogUpdateScopes(ITEM_ID cookie, const sLogScope* scopes, uint32_t count);

    /**
     * \brief   The callback of the event triggered when receive message to log.
     * \param   logMessage  The structure of the message to log.
     **/
    static void _callbackLogLogMessage(const sLogMessage* logMessage);

    /**
     * \brief   The callback of the event triggered when receive remote message to log.
     *          The buffer indicates to the NELogging::sLogMessage structure.
     * \param   logBuffer   The pointer to the NELogging::sLogMessage structure to log messages.
     * \param   size        The size of the buffer with log message.
     **/
    static void _callbackLogLogMessageEx(const unsigned char* logBuffer, uint32_t size);

    static  LogObserverBase* _theLogObserver; //!< The instance of the log observer base class.

private:
};

#endif // AREG_AREGLOGGER_CLIENT_LOGOBSERVERBASE_HPP
