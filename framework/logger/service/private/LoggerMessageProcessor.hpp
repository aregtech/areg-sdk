#ifndef AREG_LOGGER_SERVICE_PRIVATE_LOGGERMESSAGEPROCESSOR_HPP
#define AREG_LOGGER_SERVICE_PRIVATE_LOGGERMESSAGEPROCESSOR_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        logger/service/private/LoggerMessageProcessor.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, Log Collector message processor
 ************************************************************************/

 /************************************************************************
  * Include files.
  ************************************************************************/
#include "areg/base/GEGlobal.h"

#include "areg/component/NEService.hpp"
#include "areg/base/TEArrayList.hpp"
#include "extend/service/ServiceCommunicatonBase.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class LoggerServerService;
class RemoteMessage;

//////////////////////////////////////////////////////////////////////////
// LoggerMessageProcessor class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The logger message processor. It is a helper class used by
 *          logger service to process the messages.
 **/
class LoggerMessageProcessor
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Initializes the logger  message processor. Requires the instance of logger service.
     **/
    LoggerMessageProcessor(LoggerServerService & loggerService);

    ~LoggerMessageProcessor(void) = default;

//////////////////////////////////////////////////////////////////////////
// Actions
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Processes the message to query instances. The message is sent by observers to receive
     *          the list of connected non-observer instances.
     * \param   msgReceived     The message to process.
     **/
    void queryConnectedInstances(const RemoteMessage & msgReceived) const;

    /**
     * \brief   Creates a communication message with the information of connected instances
     *          and sends the notification to the specified observer target. If target is NEService::COOKIE_ANY
     *          the notification is sent to all connected observers. Otherwise, it send to
     *          the specified exact target.
     * \param   instances   The list of connected instances to include in the notification message.
     * \param   target      The ID of the target to send the message. If target is NEService::COOKIE_ANY,
     *                      the notification message is sent to all observers. Otherwise, it is sent to the exact target.
     **/
    void notifyConnectedInstances(const NEService::MapInstances& instances, const ITEM_ID & target = NEService::COOKIE_ANY) const;

    /**
     * \brief   Creates a communication message with the IDs of disconnected instances
     *          and sends the notification to the specified observer target. If target is NEService::COOKIE_ANY
     *          the notification is sent to all connected observers. Otherwise, it send to
     *          the specified exact target.
     * \param   listIds     The list of IDs of disconnected instances to include in the notification message.
     * \param   target      The ID of the target to send the message. If target is NEService::COOKIE_ANY,
     *                      the notification message is sent to all observers. Otherwise, it is sent to the exact target.
     **/
    void notifyDisconnectedInstances(const TEArrayList<ITEM_ID> & listIds, const ITEM_ID& target = NEService::COOKIE_ANY) const;

    /**
     * \brief   Called when a connected instance of application requests to register scopes.
     *          The scopes contain names and message priorities to log.
     *          The message is forwarded to the all connected observers to register scopes.
     * \param   msgReceived     The message to process.
     **/
    void registerScopesAtObserver(const RemoteMessage & msgReceived) const;

    /**
     * \brief   Called when a connected instance of observer requests to update scopes
     *          of the certain connected non-observer application.
     *          The scopes contain names and message priorities to log.
     *          The message is forwarded to the all connected observers to register scopes.
     * \param   msgReceived     The message to process.
     **/
    void updateLogSourceScopes(const RemoteMessage & msgReceived) const;

    /**
     * \brief   Called when a connected instance of observer queries the list of scopes.
     *          The message is forwarded either to all connected non-observer instances
     *          or to the certain application to receive list of log scope of the application.
     * \param   msgReceived     The message to process.
     **/
    void queryLogSourceScopes(const RemoteMessage & msgReceived) const;

    /**
     * \brief   Called when a connected instance of observer requests the clients to save log configuration,
     *          so that on next start clients start with the actual state (list of active scopes and log priorities).
     *          The message is forwarded either to all connected non-observer instances
     *          or to the certain application to save the log configuration.
     * \param   msgReceived     The message to process.
     **/
    void saveLogSourceConfiguration(const RemoteMessage & msgReceived) const;

    /**
     * \brief   Called to forward the log message to the observer application.
     * \param   msgReceived     The message to process.
     **/
    void logMessage(const RemoteMessage & msgReceived) const;

    /**
     * \brief   Checks whether the specified message source is considered as a log source.
     *          The log source application as well has list of scopes.
     *          The clients, test application and simulations are considered as a source of logs.
     *          Observers and services are not.
     * \param   msgSource   The source of message to check.
     * \return  Returns true if the specified type is a source of log messages.
     **/
    static bool isLogSource(NEService::eMessageSource msgSource);

    /**
     * \brief   Checks whether the specified message source is considered as an observer.
     *          The observers can collect logs and send instructions to update log scopes and priorities.
     * \param   msgSource   The source of message to check.
     * \return  Returns true if the specified type is an observer.
     **/
    static bool isLogObserver(NEService::eMessageSource msgSource);

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   Forwards specified message to the log sources, i.e. clients.
     *          If the target in the remote message is NEService::COOKIE_ANY, the message is sent to all clients.
     * \param   msgReceived     The remote message received from observer or generated by the logger.
     **/
    inline void _forwardMessageToLogSources(const RemoteMessage& msgReceived) const;

    /**
     * \brief   Forwards specified message to the log observer.
     *          If the target in the remote message is NEService::COOKIE_ANY, the message is sent to all observers.
     * \param   msgReceived     The remote message received from a client.
     **/
    inline void _forwardMessageToObservers(const RemoteMessage& msgReceived) const;
//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    //!< The instance of the logger service.
    LoggerServerService &   mLoggerService;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    LoggerMessageProcessor(void) = delete;
    DECLARE_NOCOPY_NOMOVE(LoggerMessageProcessor);
};

#endif // AREG_LOGGER_SERVICE_PRIVATE_LOGGERMESSAGEPROCESSOR_HPP
