/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/trace/private/NetTcpLogger.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, TCP/IP Logger object to log message into the
 *              remote object.
 ************************************************************************/

 /************************************************************************
  * Include files.
  ************************************************************************/
#include "areg/trace/private/NetTcpLogger.hpp"
#include "areg/trace/private/TraceManager.hpp"

#include "areg/base/SynchObjects.hpp"

NetTcpLogger::NetTcpLogger(LogConfiguration& tracerConfig)
    : LoggerBase        (tracerConfig)
    , IEThreadConsumer  ( )
    , mSocket           ( )
    , mThreadRetry      ( static_cast<IEThreadConsumer &>(self()), NetTcpLogger::THREAD_NAME_RETRY)
    , mRingStack        ( NetTcpLogger::RING_STACK_MAX_SIZE, NECommon::eRingOverlap::ShiftOnOverlap )
{
}

NetTcpLogger::~NetTcpLogger(void)
{
}

bool NetTcpLogger::openLogger(void)
{
    bool result{ false };

    mThreadRetry.destroyThread(NECommon::DO_NOT_WAIT);

    if (mSocket.isValid() == false)
    {
        const LogConfiguration& logConfig   = getTraceConfiguration();
        const TraceProperty& propTcpHost    = logConfig.getRemoteTcpHost();
        const TraceProperty& propTcpPort    = logConfig.getRemoteTcpPort();
        if (propTcpHost.isValid() && propTcpPort.isValid())
        {
            String host(static_cast<const String&>(propTcpHost.getValue()));
            uint16_t port(static_cast<const uint16_t>(propTcpPort.getValue()));
            if (mSocket.createSocket(host, port))
            {
                result  = true;
            }
            else
            {
                mSocket.closeSocket();
                mThreadRetry.createThread(NECommon::WAIT_INFINITE);
            }
        }
        else
        {
            OUTPUT_DBG("Ignore to establish TCP/IP remote connection, no property is set.");
            result = false;
        }
    }
    else
    {
        result = true;
    }

    return result;
}

void NetTcpLogger::closeLogger(void)
{
    mSocket.closeSocket();
    mThreadRetry.destroyThread(NECommon::WAIT_INFINITE);
    while (mRingStack.isEmpty() == false)
    {
        NETrace::sLogMessage * header = mRingStack.popFirst();
        delete header;
    }

    mRingStack.clear();
}

void NetTcpLogger::logMessage(const NETrace::sLogMessage& logMessage)
{
    if (mSocket.isValid())
    {
        if (logMessage.lmHeader.hdrCookie >= NETrace::COOKIE_FIRST_VALID)
        {
            if (mSocket.sendData(reinterpret_cast<const unsigned char*>(&logMessage), sizeof(NETrace::sLogMessage) ) == 0)
            {
                if (mSocket.isAlive() == false)
                {
                    TraceManager::setCookie(NETrace::COOKIE_LOCAL);
                    mSocket.closeSocket();
                    mThreadRetry.createThread(NECommon::WAIT_INFINITE);
                }
            }
        }
        else if (logMessage.lmHeader.hdrCookie == NETrace::COOKIE_ANY)
        {
            NETrace::sLogMessage* log = new NETrace::sLogMessage;
            *log = logMessage;
            mRingStack.pushLast(log);
        }
        else
        {
            ASSERT(false);
        }
    }
}

void NetTcpLogger::flushLogs(void)
{
    while (mRingStack.isEmpty() == false)
    {
        NETrace::sLogMessage* log = mRingStack.popFirst();
        ASSERT(log != nullptr);
        mSocket.sendData(reinterpret_cast<const unsigned char*>(log), sizeof(NETrace::sLogMessage));
        delete log;
    }
}

bool NetTcpLogger::isLoggerOpened(void) const
{
    return mSocket.isValid();
}

void NetTcpLogger::onThreadRuns(void)
{
    SynchTimer timer(NetTcpLogger::TIMEOUT_CONNECT_RETRY, false, false, true);
    timer.setTimer();
    timer.lock(NECommon::WAIT_INFINITE);
    TraceManager::retryStartLogging();
}
