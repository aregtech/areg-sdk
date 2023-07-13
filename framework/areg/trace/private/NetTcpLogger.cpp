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

#include "areg/base/SharedBuffer.hpp"
#include "areg/base/SynchObjects.hpp"

NetTcpLogger::NetTcpLogger(LogConfiguration& tracerConfig)
    : LoggerBase        (tracerConfig)
    , IEThreadConsumer  ( )

    , mSocket           ( )
    , mRingStack        ( NetTcpLogger::RING_STACK_MAX_SIZE, NECommon::eRingOverlap::ShiftOnOverlap )
    , mConnectionState  ( eConnectionStates::StateInactive )
    , mRecvThread       ( static_cast<IEThreadConsumer &>(self()), LOG_RECEIVE_THREAD_NAME )
    , mEventExit        ( true, false )
    , mLock             ( false )
{
}

bool NetTcpLogger::openLogger(void)
{
    Lock lock( mLock );
    bool result{ false };

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
                mConnectionState = eConnectionStates::StateConnected;
                result = true;
                mRecvThread.createThread( NECommon::WAIT_INFINITE );
            }
            else
            {
                OUTPUT_WARN( "Failed to establish TCP/IP connection with remote logging service." );
                closeConnection( );
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
    Lock lock( mLock );

    mEventExit.setEvent( );
    _closeConnection( );
    mRecvThread.destroyThread( NECommon::WAIT_INFINITE );
    mEventExit.resetEvent( );

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
    Lock lock( mLock );
    return (mConnectionState != eConnectionStates::StateInactive);
}

bool NetTcpLogger::sendData(const SharedBuffer & data)
{
    const int size{ static_cast<int>(data.getSizeUsed()) };
    return (mSocket.isValid() && (mSocket.sendData(data.getBuffer(), size) == size));
}

void NetTcpLogger::closeConnection( void )
{
    Lock lock( mLock );
    _closeConnection( );
}

void NetTcpLogger::setActive( bool isActive )
{
    Lock lock( mLock );
    mConnectionState = eConnectionStates::StateActive;
}

bool NetTcpLogger::isActive( void ) const
{
    Lock lock( mLock );
    return (mConnectionState == eConnectionStates::StateActive);
}

inline void NetTcpLogger::_closeConnection( void )
{
    mConnectionState = eConnectionStates::StateInactive;
    mSocket.closeSocket( );
}

void NetTcpLogger::onThreadRuns( void )
{
    Lock checkExit( mEventExit, false );
    bool lostConnection{ false };

    while ( true )
    {
        if ( checkExit.lock(NECommon::DO_NOT_WAIT) == false)
        {
            NETrace::sLogHeader logHeader{ };
            int sizeReceived = mSocket.receiveData( reinterpret_cast<unsigned char *>(&logHeader), sizeof( NETrace::sLogHeader ) );

            if ( sizeReceived <= 0 )
            {
                lostConnection = true;
                break;
            }
            else
            {
                SharedBuffer data( logHeader.hdrDataLen + sizeof( logHeader ) );
                data << logHeader;
                unsigned char * buffer = data.getBuffer( ) + sizeof( logHeader );
                if ( mSocket.receiveData( buffer, logHeader.hdrDataLen ) < static_cast<int>(logHeader.hdrDataLen) )
                {
                    lostConnection = true;
                    break;
                }

                TraceManager::netReceivedData( data );
            }
        }
        else
        {
            mEventExit.resetEvent( );
            break;
        }
    };

    if ( lostConnection && isLoggerOpened( ) )
    {
        closeConnection( );
        TraceManager::netConnectionLost( );
    }
}
