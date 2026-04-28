/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        aregextend/service/private/ClientConnectionPair.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, pool send + receive thread pair implementation.
 ************************************************************************/
#include "aregextend/service/private/ClientConnectionPair.hpp"

namespace areg::ext {

ClientConnectionPair::ClientConnectionPair( areg::RemoteMessageHandler & remoteService
                                          , ServerConnection & connection
                                          , ServerSendThread & globalSend
                                          , ServerReceiveThread & globalRecv
                                          , std::string_view sendName
                                          , std::string_view recvName )
    : mSendThread   ( self(), remoteService, connection, globalSend, sendName )
    , mReceiveThread( remoteService, connection, globalRecv, recvName )
    , mConnections  ( )
    , mLock         ( )
{
}

bool ClientConnectionPair::start()
{
    if ( !mSendThread.start(areg::WAIT_INFINITE) || !mSendThread.wait_start(areg::WAIT_INFINITE) )
        return false;

    if ( !mReceiveThread.start(areg::WAIT_INFINITE) || !mReceiveThread.wait_start(areg::WAIT_INFINITE) )
    {
        // Roll back: stop send thread before returning failure.
        mSendThread.trigger_exit();
        mSendThread.shutdown(areg::WAIT_INFINITE);
        return false;
    }

    return true;
}

void ClientConnectionPair::stop()
{
    mConnections.clear();
    stop_send();
    stop_recv();
}

void ClientConnectionPair::stop_send()
{
    if ( mSendThread.is_running() )
    {
        mSendThread.trigger_exit();
    }

    mSendThread.shutdown(areg::WAIT_INFINITE);
}

void ClientConnectionPair::stop_recv()
{
    if ( mReceiveThread.is_running() )
    {
        mReceiveThread.request_stop();
    }

    mReceiveThread.shutdown(areg::WAIT_INFINITE);
}

} // namespace areg::ext
