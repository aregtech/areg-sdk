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
#include "areg/base/IEThreadConsumer.hpp"

#include "areg/base/IEIOStream.hpp"
#include "areg/base/TERingStack.hpp"
#include "areg/base/Thread.hpp"
#include "areg/base/SocketClient.hpp"
#include "areg/base/String.hpp"
#include "areg/base/SynchObjects.hpp"

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
                    , private   IEThreadConsumer
{
//////////////////////////////////////////////////////////////////////////
// Internal types and constants.
//////////////////////////////////////////////////////////////////////////
private:
    //!< The ring buffer of logging message to queue if logging service is not available.
    using RingStack = TENolockRingStack<NETrace::sLogMessage *>;
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
     **/
    explicit NetTcpLogger(LogConfiguration& tracerConfig);

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
     * \brief   Call to flush logs, if they are queued. Some loggers might ignore this.
     **/
    virtual void flushLogs( void ) override;

    /**
     * \brief   Returns true if logger is initialized (opened).
     **/
    virtual bool isLoggerOpened( void ) const override;

//////////////////////////////////////////////////////////////////////////
// Operations:
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Sends the specified data over opened TCP/IP connection.
     * \param   data    The data to send.
     * \return  Returns true if succeeded to send the data. Otherwise, returns false.
     **/
    bool sendData(const SharedBuffer & data);

    /**
     * \brief   Closes the connection socket. And does not remote the logs from the pending stacks.
     **/
    void closeConnection( void );

    /**
     * \brief   Sets the active state of the TCP logger.
     **/
    void setActive( bool isActive );

    /**
     * \brief   Returns true if connection state is not Inactive
     **/
    bool isActive( void ) const;

    /**
     * \brief   Sets the IP-address of the log host sent by remote logging service.
     * \param   ipAddr      The identified IP-address of the logs host.
     **/
    void setHostIpAddress( const String & ipAddr );

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
/************************************************************************/
// IEThreadConsumer interface overrides
/************************************************************************/

    /**
     * \brief   This callback function is called from Thread object, when it is 
     *          running and fully operable. If thread needs run in loop, the loop 
     *          should be implemented here. When consumer exits this function, 
     *          the thread will complete work. To restart thread running, 
     *          createThread() method should be called again.
     **/
    virtual void onThreadRuns( void ) override;

/************************************************************************/
// Internal methods
/************************************************************************/

    //!< Closes connection
    void _closeConnection( void );

    //!< Wrapper of 'this' pointer.
    inline NetTcpLogger& self(void);

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    //!< The TCP/IP client socket to connect to remote host.
    SocketClient        mSocket;
    //!< The ring stack to queue log messages if the connection setup did not complete yet.
    RingStack           mRingStack;
    //!< The IP-address of the host. This is set by remote logging service.
    String              mHostIpAddr;
    //!< The connection states.
    eConnectionStates   mConnectionState;
    //!< brief   The data receiver thread.
    Thread              mRecvThread;
    //!< The synchronization event to trigger exit of receive thread.
    SynchEvent          mEventExit;
    //!< Synchronization object.
    mutable Mutex       mLock;

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
