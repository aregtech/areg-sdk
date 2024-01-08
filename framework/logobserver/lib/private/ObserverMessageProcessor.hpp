#ifndef AREG_LOGOBSERVER_LIB_PRIVATE_OBSERVERMESSAGEPROCESSOR_HPP
#define AREG_LOGOBSERVER_LIB_PRIVATE_OBSERVERMESSAGEPROCESSOR_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        logobserver/lib/private/ObserverMessageProcessor.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, Log Observer library API.
 *              The log observer message processor.
 ************************************************************************/

 /************************************************************************
  * Include files.
  ************************************************************************/
#include "logobserver/lib/LogObserverSwitches.h"

/************************************************************************
 * Dependencies
 ************************************************************************/
class LoggerClient;
class RemoteMessage;

//////////////////////////////////////////////////////////////////////////
// ObserverMessageProcessor class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   An object to process all received messages.
 **/
class ObserverMessageProcessor
{
//////////////////////////////////////////////////////////////////////////
// Default constructor and destructor.
//////////////////////////////////////////////////////////////////////////
public:
    ObserverMessageProcessor(LoggerClient& loggerClient);
    ~ObserverMessageProcessor(void) = default;

//////////////////////////////////////////////////////////////////////////
// Operations.
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Triggered to notify the connection of the service.
     *          The message contains Cookie ID of the observer.
     * \param   msgReceived     The buffer with data of the message received when connect to the service.
     **/
    void notifyServiceConnection(const RemoteMessage& msgReceived);

    /**
     * \brief   Triggered to notify the list of connected clients.
     *          This message is sent as a reply on query the connected clients.
     * \param   msgReceived     The buffer with data of connected clients.
     **/
    void notifyConnectedClients(const RemoteMessage& msgReceived);

    /**
     * \brief   Triggered to notify the list of scopes, scope ID and the scope message priority.
     *          This message is sent as a reply on query the list of scopes.
     * \param   msgReceived     The buffer with data of scope names, scope IDs and the message priority.
     *                          This contains the information of all scopes.
     **/
    void notifyLogRegisterScopes(const RemoteMessage& msgReceived);

    /**
     * \brief   Triggered to notify to log a message.
     * \param   msgReceived     The buffer with the log message.
     **/
    void notifyLogMessage(const RemoteMessage& msgReceived);

//////////////////////////////////////////////////////////////////////////
// Hidden members.
//////////////////////////////////////////////////////////////////////////
private:
    LoggerClient &  mLoggerClient;  //!< The object of the observer client.

//////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    ObserverMessageProcessor(void) = delete;
    DECLARE_NOCOPY_NOMOVE(ObserverMessageProcessor);
};

#endif  // AREG_LOGOBSERVER_LIB_PRIVATE_OBSERVERMESSAGEPROCESSOR_HPP
