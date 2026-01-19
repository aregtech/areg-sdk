#ifndef AREG_LOGGING_PRIVATE_NETTCPLOGGER_HPP
#define AREG_LOGGING_PRIVATE_NETTCPLOGGER_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/logging/private/NetTcpLogger.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, TCP/IP logging object to log message into the
 *              remote object.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/logging/private/LoggerBase.hpp"
#include "areg/ipc/ServiceClientConnectionBase.hpp"
#include "areg/ipc/IEServiceConnectionConsumer.hpp"
#include "areg/ipc/IERemoteMessageHandler.hpp"

#include "areg/base/IEIOStream.hpp"
#include "areg/base/TERingStack.hpp"
#include "areg/base/Thread.hpp"
#include "areg/base/String.hpp"
#include "areg/base/SyncObjects.hpp"
#include "areg/ipc/ClientConnection.hpp"

#include <string_view>

#if AREG_LOGS

/************************************************************************
 * Dependencies
 ************************************************************************/
class DispatcherThread;
class LogConfiguration;
class ScopeController;
class SharedBuffer;

//////////////////////////////////////////////////////////////////////////
// NetTcpLogger class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Network TCP/IP logging object to log messages to remote device.
 *          The object uses TCP/IP connection to the remote log collector service
 *          and forwards log messages to the remote service.
 **/
class NetTcpLogger  : public    LoggerBase
                    , public    ServiceClientConnectionBase
                    , private   IEServiceConnectionConsumer
                    , private   IERemoteMessageHandler
{
//////////////////////////////////////////////////////////////////////////
// Internal types and constants.
//////////////////////////////////////////////////////////////////////////
private:
    //!< The ring buffer of logging message to queue if logging service is not available.
    using RingStack = TENolockRingStack<RemoteMessage>;

    //!< A prefix to add in front of thread and timer names.
    static constexpr std::string_view   PREFIX_THREAD{ "logger_" };

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializes the network logging client object to forward logs to the log collector service.
     * \param   logConfig       The log configuration object, which contains information about log state.
     * \param   scopeController The scope controller object, which contains and controls the scopes in application.
     * \param   dispatchThread  The dispatcher thread to dispatch events and messages.
     **/
    NetTcpLogger(LogConfiguration & logConfig, ScopeController & scopeController, DispatcherThread & dispatchThread);

    /**
     * \brief   Destructor.
     **/
    virtual ~NetTcpLogger(void) = default;

//////////////////////////////////////////////////////////////////////////
// Override operations and attribute
//////////////////////////////////////////////////////////////////////////
public:

/************************************************************************/
// LoggerBase overrides
/************************************************************************/

    /**
     * \brief   Initializes and opens the logger. If this method returns true,
     *          the log manager will start forwarding messages for logging.
     *          If it returns false, the log manager assumes the logger is not
     *          initialized and will not send messages for logging.
     *          The logger must be opened before any messages can be logged.
     * \return  Returns true if the logger was successfully initialized and opened.
     **/
    virtual bool openLogger( void ) override;

    /**
     * \brief   Called to close logger and stop logging.
     **/
    virtual void closeLogger( void ) override;

    /**
     * \brief   Called when message should be logged.
     *          Every logger should implement method to process logger specific logging.
     **/
    virtual void logMessage( const NELogging::sLogMessage & logMessage ) override;

    /**
     * \brief   Returns true if logger is initialized (opened).
     **/
    virtual bool isLoggerOpened( void ) const override;

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
private:

/************************************************************************/
// IEServiceConnectionConsumer overrides
/************************************************************************/

    /**
     * \brief   Triggered when remote service connection and communication channel is established.
     * \param   channel     The connection and communication channel of remote service.
     **/
    virtual void connectedRemoteServiceChannel(const Channel& channel) override;

    /**
     * \brief   Triggered when disconnected remote service connection and communication channel.
     * \param   channel     The connection and communication channel of remote service.
     **/
    virtual void disconnectedRemoteServiceChannel(const Channel& channel) override;

    /**
     * \brief   Triggered when remote service connection and communication channel is lost.
     *          The connection is considered lost if it not possible to read or
     *          receive data, and it was not stopped by API call.
     * \param   channel     The connection and communication channel of remote service.
     **/
    virtual void lostRemoteServiceChannel(const Channel& channel) override;

/************************************************************************/
// IERemoteMessageHandler interface overrides
/************************************************************************/

    /**
     * \brief   Triggered, when failed to send message.
     * \param   msgFailed   The message, which failed to send.
     * \param   whichTarget The target socket to send message.
     **/
    virtual void failedSendMessage( const RemoteMessage & msgFailed, Socket & whichTarget ) override;

    /**
     * \brief   Triggered, when failed to receive message.
     * \param   whichSource Indicates the failed source socket to receive message.
     **/
    virtual void failedReceiveMessage( Socket & whichSource ) override;

    /**
     * \brief   Triggered, when failed to process message, i.e. the target for message processing was not found.
     *          In case of request message processing, the source should receive error notification.
     * \param   msgUnprocessed  Unprocessed message data.
     **/
    virtual void failedProcessMessage( const RemoteMessage & msgUnprocessed ) override;

    /**
     * \brief   Triggered, when need to process received message.
     * \param   msgReceived Received message to process.
     * \param   whichSource The source socket, which received message.
     **/
    virtual void processReceivedMessage( const RemoteMessage & msgReceived, Socket & whichSource ) override;

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
/************************************************************************/
// Internal methods
/************************************************************************/

    //!< Wrapper of 'this' pointer.
    inline NetTcpLogger& self(void);

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    //!< The instance of scope controller
    ScopeController &   mScopeController;
    //!< The flag, indicating whether the TPC/IP network logging is enabled or not.
    bool                mIsEnabled;
    //!< The ring stack to queue log messages if the connection setup did not complete yet.
    RingStack           mRingStack;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    NetTcpLogger( void ) = delete;
    DECLARE_NOCOPY_NOMOVE(NetTcpLogger);
};

//////////////////////////////////////////////////////////////////////////
// NetTcpLogger inline methods.
//////////////////////////////////////////////////////////////////////////

inline NetTcpLogger& NetTcpLogger::self(void)
{
    return (*this);
}

#endif  // AREG_LOGS
#endif  // AREG_LOGGING_PRIVATE_NETTCPLOGGER_HPP
