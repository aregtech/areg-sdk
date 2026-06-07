#ifndef AREG_LOGGING_PRIVATE_NETTCPLOGGER_HPP
#define AREG_LOGGING_PRIVATE_NETTCPLOGGER_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/logging/private/NetTcpLogger.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, TCP/IP logging object to log message into the
 *              remote object.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/logging/private/LoggerBase.hpp"
#include "areg/ipc/ServiceClientConnectionBase.hpp"
#include "areg/ipc/ConnectionConsumer.hpp"
#include "areg/ipc/RemoteMessageHandler.hpp"

#include "areg/base/IOStream.hpp"
#include "areg/base/RingStack.hpp"
#include "areg/base/Thread.hpp"
#include "areg/base/String.hpp"
#include "areg/base/SyncPrimitives.hpp"
#include "areg/ipc/ClientConnection.hpp"

#include <string_view>
#if AREG_LOGGING

/************************************************************************
 * Dependencies
 ************************************************************************/
namespace areg {

    class DispatcherThread;
    class LogConfiguration;
    class ScopeController;
    class SharedBuffer;
} // namespace areg

namespace areg {

//////////////////////////////////////////////////////////////////////////
// NetTcpLogger class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Network TCP/IP logging object to log messages to remote device.
 *          The object uses TCP/IP connection to the remote log collector service
 *          and forwards log messages to the remote service.
 **/
class NetTcpLogger final    : public    LoggerBase
                            , public    ServiceClientConnectionBase
                            , private   ConnectionConsumer
                            , private   RemoteMessageHandler
{
//////////////////////////////////////////////////////////////////////////
// Internal types and constants.
//////////////////////////////////////////////////////////////////////////
private:
    //!< The ring buffer of logging message to queue if logging service is not available.
    using PendingQueue = RingStack<MessageEnvelope>;

    //!< A prefix to add in front of thread and timer names.
    static constexpr std::string_view   PREFIX_THREAD{ "logger_" };

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializes the network logging client object to forward logs to the log collector
     *          service.
     *
     * \param   logConfig           The log configuration object, which contains information about
     *                              log state.
     * \param   scopeController     The scope controller object, which contains and controls the
     *                              scopes in application.
     * \param   dispatchThread      The dispatcher thread to dispatch events and messages.
     **/
    NetTcpLogger(LogConfiguration & logConfig, ScopeController & scopeController, DispatcherThread & dispatchThread);

    ~NetTcpLogger() override = default;

//////////////////////////////////////////////////////////////////////////
// Override operations and attribute
//////////////////////////////////////////////////////////////////////////
public:

/************************************************************************/
// LoggerBase overrides
/************************************************************************/

    /**
     * \brief   Initializes and opens the logger. If this method returns true, the log manager will
     *          start forwarding messages for logging. If it returns false, the log manager assumes
     *          the logger is not initialized and will not send messages for logging. The logger
     *          must be opened before any messages can be logged.
     *
     * \return  Returns true if the logger was successfully initialized and opened.
     **/
    bool open_logger() final;

    /**
     * \brief   Closes the logger and stops logging.
     **/
    void close_logger() final;

    /**
     * \brief   Called when message should be logged. Every logger should implement method to
     *          process logger specific logging.
     *
     * \param   logMessage     The logging message to process.
     **/
    void log_message( const areg::LogEntry & logMessage) final;

    /**
     * \brief   Forwards a pre-built event envelope to the log collector without re-constructing it.
     *
     * \param   msg     Pre-built event envelope; must have is_valid() == true.
     **/
    void forward_message( const areg::MessageEnvelope & msg );

    /**
     * \brief   Forwards a pre-built event envelope to the log collector without re-constructing it.
     *
     * \param   msg     Pre-built event envelope; must have is_valid() == true.
     **/
    void forward_message(areg::MessageEnvelope&& msg);

    /**
     * \brief   Returns true if logger is initialized (opened).
     **/
    [[nodiscard]]
    bool is_logger_opened() const noexcept final;

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
private:

/************************************************************************/
// ConnectionConsumer overrides
/************************************************************************/

    /**
     * \brief   Triggered when remote service connection and communication channel is established.
     *
     * \param   channel     The connection and communication channel of remote service.
     **/
    void on_service_channel_connected(const Channel& channel) final;

    /**
     * \brief   Triggered when disconnected remote service connection and communication channel.
     *
     * \param   channel     The connection and communication channel of remote service.
     **/
    void on_service_channel_disconnected(const Channel& channel) final;

    /**
     * \brief   Triggered when remote service connection and communication channel is lost. The
     *          connection is considered lost if it not possible to read or receive data, and it was
     *          not stopped by API call.
     *
     * \param   channel     The connection and communication channel of remote service.
     **/
    void on_service_channel_lost(const Channel& channel) final;

/************************************************************************/
// RemoteMessageHandler interface overrides
/************************************************************************/

    /**
     * \brief   Triggered when failed to send message.
     *
     * \param   msgFailed       The message, which failed to send.
     * \param   whichTarget     The target socket to send message.
     **/
    void failed_send_message( const MessageEnvelope & msgFailed, Socket & whichTarget ) final;

    /**
     * \brief   Triggered when failed to receive message.
     *
     * \param   whichSource     Indicates the failed source socket to receive message.
     **/
    void failed_receive_message( Socket & whichSource ) final;

    /**
     * \brief   Triggered when failed to process message, i.e. the target for message processing was
     *          not found. In case of request message processing, the source should receive error
     *          notification.
     *
     * \param   msgUnprocessed      Unprocessed message data.
     **/
    void failed_process_message( const MessageEnvelope & msgUnprocessed ) final;

    /**
     * \brief   Triggered when need to process received message.
     *
     * \param   msgReceived     Received message to process.
     * \param   whichSource     The source socket, which received message.
     **/
    void process_received_message( MessageEnvelope & msgReceived, Socket & whichSource ) final;

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
/************************************************************************/
// Internal methods
/************************************************************************/

    //!< Wrapper of 'this' pointer.
    inline NetTcpLogger& self();

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    //!< The instance of scope controller
    ScopeController &   mScopeController;
    //!< The flag, indicating whether the TPC/IP network logging is enabled or not.
    std::atomic<bool>   mIsEnabled;
    //!< The ring stack to queue log messages if the connection setup did not complete yet.
    PendingQueue        mRingStack;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    NetTcpLogger() = delete;
    AREG_NOCOPY_NOMOVE(NetTcpLogger);
};

//////////////////////////////////////////////////////////////////////////
// NetTcpLogger inline methods.
//////////////////////////////////////////////////////////////////////////

inline NetTcpLogger& NetTcpLogger::self()
{
    return (*this);
}

} // namespace areg

#endif  // AREG_LOGGING
#endif  // AREG_LOGGING_PRIVATE_NETTCPLOGGER_HPP
