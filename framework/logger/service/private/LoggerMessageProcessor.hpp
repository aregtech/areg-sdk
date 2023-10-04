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
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform, Log Collector message processor
 ************************************************************************/

 /************************************************************************
  * Include files.
  ************************************************************************/
#include "areg/base/GEGlobal.h"

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
     * \brief   Sends the notification message to the specified target.
     *          Sets in the message as a source the ID of the logger service and the target
     *          is passed as parameter.
     * \param   target      The ID of the target application.
     * \param   msgNotify   The message to send to the target.
     **/
    void notifyTarget(const ITEM_ID & target, const RemoteMessage & msgNotify) const;

    /**
     * \brief   Processes the message to query instances. The message is sent by observers to receive
     *          the list of connected non-observer instances.
     * \param   msgReceived     The message to process.
     **/
    void queryInstances(const RemoteMessage & msgReceived) const;

    /**
     * \brief   Notifies the observers about connected instances. Automatically processed when
     *          new instance of application is connected.
     * \param   msgReceived     The message to process.
     **/
    void notifyInstances(void) const;

    /**
     * \brief   Notifies the targeted observer about connected instances.
     * \param   msgReceived     The message to process.
     **/
    void notifyTargetInstances(const ITEM_ID & target) const;

    /**
     * \brief   Called when a connected instance of application requests to register scopes.
     *          The scopes contain names and message priorities to log.
     *          The message is forwarded to the all connected observers to register scopes.
     * \param   msgReceived     The message to process.
     **/
    void registerScopes(const RemoteMessage & msgReceived) const;

    /**
     * \brief   Called when a connected instance of observer requests to update scopes
     *          of the certain connected non-observer application.
     *          The scopes contain names and message priorities to log.
     *          The message is forwarded to the all connected observers to register scopes.
     * \param   msgReceived     The message to process.
     **/
    void updateScopes(const RemoteMessage & msgReceived) const;

    /**
     * \brief   Called when a connected instance of observer queries the list of scopes.
     *          The message is forwarded either to all connected non-observer instances
     *          or to the certain application to receive list of log scope of the application.
     * \param   msgReceived     The message to process.
     **/
    void queryScopes(const RemoteMessage & msgReceived) const;

    /**
     * \brief   Called to forward the log message to the observer application.
     * \param   msgReceived     The message to process.
     **/
    void logMessage(const RemoteMessage & msgReceived) const;

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
