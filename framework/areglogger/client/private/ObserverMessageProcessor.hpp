#ifndef AREG_AREGLOGGER_CLIENT_PRIVATE_OBSERVERMESSAGEPROCESSOR_HPP
#define AREG_AREGLOGGER_CLIENT_PRIVATE_OBSERVERMESSAGEPROCESSOR_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areglogger/client/private/ObserverMessageProcessor.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Log Observer library API.
 *              The log observer message processor.
 ************************************************************************/

 /************************************************************************
  * Include files.
  ************************************************************************/
#include "areglogger/client/LogObserverSwitches.h"

/************************************************************************
 * Dependencies
 ************************************************************************/
namespace areg {
    struct LogEntry;
    class RemoteMessage;
} // namespace areg

namespace areg::logger {
    class LoggerClient;
}

namespace areg::logger {

//////////////////////////////////////////////////////////////////////////
// ObserverMessageProcessor class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Processor for messages received from the log service.
 **/
class ObserverMessageProcessor
{
//////////////////////////////////////////////////////////////////////////
// Default constructor and destructor.
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief
     *
     * \param   loggerClient    Reference to the LoggerClient instance to process messages for.
     **/
    ObserverMessageProcessor(LoggerClient& loggerClient);
    ~ObserverMessageProcessor() = default;

//////////////////////////////////////////////////////////////////////////
// Operations.
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Handles service connection notification with the observer's Cookie ID.
     *
     * \param   msgReceived     Buffer containing the connection message with the observer's Cookie
     *                          ID.
     **/
    void notify_service_connection(const RemoteMessage& msgReceived);

    /**
     * \brief   Handles notification of the list of connected clients in response to a query.
     *
     * \param   msgReceived     Buffer containing the data of connected clients.
     **/
    void notify_connected_clients(const RemoteMessage& msgReceived);

    /**
     * \brief   Handles notification of registered scopes with their IDs and message priorities in
     *          response to a query.
     *
     * \param   msgReceived     Buffer containing scope names, IDs, and message priorities for all
     *                          scopes.
     **/
    void notify_log_register_scopes(const RemoteMessage& msgReceived);

    /**
     * \brief   Handles notification that scope priorities have been updated.
     *
     * \param   msgReceived     Buffer containing scope names, IDs, and updated message priorities.
     **/
    void notify_log_update_scopes(const RemoteMessage& msgReceived);

    /**
     * \brief   Handles a log message notification.
     *
     * \param   msgReceived     Buffer containing the log message.
     **/
    void notify_log_message(const RemoteMessage& msgReceived);

private:

    //!< Triggered to process client connected message.
    /**
     * \brief   Handles notification when clients connect.
     *
     * \param   msgReceived     Buffer containing connected client data.
     **/
    void _clients_connected(const RemoteMessage& msgReceived);

    //!< Triggered to process client disconnected message.
    /**
     * \brief   Handles notification when clients disconnect.
     *
     * \param   msgReceived     Buffer containing disconnected client data.
     **/
    void _clients_disconnected(const RemoteMessage& msgReceived);

    //!< Initializes the local log message with default values.
    void _init_local_log_message(areg::LogEntry& log, ITEM_ID cookie, TIME64 timestamp = 0) const;

//////////////////////////////////////////////////////////////////////////
// Hidden members.
//////////////////////////////////////////////////////////////////////////
private:
    LoggerClient &  mLoggerClient;  //!< The object of the observer client.

//////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    ObserverMessageProcessor() = delete;
    AREG_NOCOPY_NOMOVE(ObserverMessageProcessor);
};

} // namespace areg::logger

#endif  // AREG_AREGLOGGER_CLIENT_PRIVATE_OBSERVERMESSAGEPROCESSOR_HPP
