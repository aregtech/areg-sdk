/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/ipc/private/ClientConnection.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform Client Connection class declaration
 ************************************************************************/
#include "areg/ipc/ClientConnection.hpp"

#include "areg/base/RemoteMessage.hpp"
#include "areg/component/ServiceDefs.hpp"

#include "areg/logging/GELog.h"

ClientConnection::ClientConnection()
    : SocketConnectionBase    ( )
    , mClientSocket ( )
    , mCookie       ( areg::COOKIE_UNKNOWN )
{
}

ClientConnection::ClientConnection(const String & hostName, unsigned short portNr)
    : SocketConnectionBase    ( )
    , mClientSocket ( hostName, portNr )
    , mCookie       ( areg::COOKIE_UNKNOWN )
{
}

ClientConnection::ClientConnection(const areg::SocketAddress & remoteAddress)
    : SocketConnectionBase    ( )
    , mClientSocket ( remoteAddress )
    , mCookie       ( areg::COOKIE_UNKNOWN )
{
}


bool ClientConnection::createSocket(const String & hostName, unsigned short portNr)
{
    setCookie( mClientSocket.createSocket(hostName, portNr) ? areg::COOKIE_LOCAL : areg::COOKIE_UNKNOWN );
    return mClientSocket.isValid();
}

bool ClientConnection::createSocket()
{
    setCookie( mClientSocket.createSocket() ? areg::COOKIE_LOCAL : areg::COOKIE_UNKNOWN );
    return mClientSocket.isValid();
}

void ClientConnection::closeSocket()
{
    setCookie(areg::COOKIE_UNKNOWN);
    mClientSocket.closeSocket();
}
