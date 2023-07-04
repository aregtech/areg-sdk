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
#include "areg/base/IEIOStream.hpp"
#include "areg/base/IEThreadConsumer.hpp"
#include "areg/trace/private/LoggerBase.hpp"

#include "areg/base/SocketClient.hpp"
#include "areg/base/String.hpp"
#include "areg/base/Thread.hpp"

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
    //!< The timeout to retry to establish the TCP/IP connection.
    static constexpr uint32_t   TIMEOUT_CONNECT_RETRY       { NECommon::TIMEOUT_1_SEC };
    //<! The retry reconnect thread name.
    static constexpr std::string_view   THREAD_NAME_RETRY   { "_LOGGER_RETRY_TCP_CONNECT_THREAD_" };
    //<! The number of messages to queue until logging service is available.
    static constexpr uint32_t           RING_STACK_MAX_SIZE { 100 };
    //!< The ring buffer of logging message to queue if logging service is not available.
    using RingStack = TENolockRingStack<NETrace::sLogMessage *>;

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
    virtual ~NetTcpLogger(void);

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
// Overrides
//////////////////////////////////////////////////////////////////////////
protected:
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

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    //!< Wrapper of 'this' pointer.
    inline NetTcpLogger& self(void);

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    //!< The TCP/IP client socket to connect to remote host.
    SocketClient    mSocket;
    //!< The thread to make retries to reconnect to logging service.
    Thread          mThreadRetry;
    //!< The ring stack to queue log messages if the connection setup did not complete yet.
    RingStack       mRingStack;

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
