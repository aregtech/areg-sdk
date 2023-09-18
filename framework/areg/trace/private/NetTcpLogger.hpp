#ifndef AREG_TRACE_PRIVATE_NETTCPLOGGER_HPP
#define AREG_TRACE_PRIVATE_NETTCPLOGGER_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/trace/private/NetTcpLogger.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, TCP/IP Logger object to log message into the
 *              remote object.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/trace/private/LoggerBase.hpp"
#include "areg/ipc/ServiceClientConnectionBase.hpp"
#include "areg/ipc/IEServiceConnectionConsumer.hpp"
#include "areg/ipc/IERemoteMessageHandler.hpp"

#include "areg/base/IEIOStream.hpp"
#include "areg/base/TERingStack.hpp"
#include "areg/base/Thread.hpp"
#include "areg/base/String.hpp"
#include "areg/base/SynchObjects.hpp"
#include "areg/ipc/ClientConnection.hpp"

#include <string_view>

class LogConfiguration;
class SharedBuffer;

//////////////////////////////////////////////////////////////////////////
// NetTcpLogger class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Message logger to log messages to remote device.
 *          At the moment the output logger supports only ASCII messages
 *          and any Unicode character might output wrong.
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
    //!< The state of the TCP logger
    enum eConnectionStates
    {
          StateInactive     = 0 //!< The logger is inactive.
        , StateConnected    = 1 //!< The logger is connected.
        , StateActive       = 3 //!< The logger is active and can send-receive message.
    };

    //!< The number of messages to queue until logging service is available.
    static constexpr uint32_t           RING_STACK_MAX_SIZE     { 100 };
    //!< The name of logging thread, which receive messages from logging service.
    static constexpr std::string_view   LOG_RECEIVE_THREAD_NAME { "_LOG_NET_RECEIVE_THREAD_" };

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Instantiates logger and sets tracer configuration object,
     *          which contains methods to get property values after
     *          configuring tracer.
     * \param   tracerConfig    The instance tracer configuration object,
     *                          which contains configuration values,
     *                          required by logger during initialization (open)
     *                          and when outputs message.
     * \param   dispatchThread  The thread to dispatch the messages.
     **/
    explicit NetTcpLogger(LogConfiguration& tracerConfig, DispatcherThread & dispatchThread);

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
     * \brief   Called to initialize / open logger. If method returns true, 
     *          the trace manager starts to forward messages for logging.
     *          If method returns false, the tracer manager assumes that
     *          logger is not initialized and will not send messages for logging.
     *          Before any message is logger, the logger should be opened.
     * \return  Returns true if logger succeeded initialization (open).
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
    virtual void logMessage( const NETrace::sLogMessage & logMessage ) override;

    /**
     * \brief   Returns true if logger is initialized (opened).
     **/
    virtual bool isLoggerOpened( void ) const override;

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
private:

/************************************************************************/
// IEServiceConnectionProvider overrides
/************************************************************************/
    /**
     * \brief   Creates the service connect request message, sets the message target and the source.
     * \param   source  The ID of the source that sends connection message request.
     * \param   target  The ID of the target to send the connection message request.
     * \return  Returns the created message for remote communication.
     **/
    virtual RemoteMessage createServiceConnectMessage(  const ITEM_ID & source, const ITEM_ID & target  ) const override;

    /**
     * \brief   Creates the service disconnect request message, sets the message target and the source.
     * \param   source  The ID of the source that sends the disconnect message request.
     * \param   target  The ID of the target to send the disconnection message request.
     * \return  Returns the created message for remote communication.
     **/
    virtual RemoteMessage createServiceDisconnectMessage(  const ITEM_ID & source, const ITEM_ID & target  ) const override;

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

#endif  // AREG_TRACE_PRIVATE_NETTCPLOGGER_HPP
