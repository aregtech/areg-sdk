/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/ipc/ClientConnection.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform Client Connection class declaration
 ************************************************************************/
#include "areg/ipc/private/ClientConnection.hpp"

#include "areg/ipc/NEConnection.hpp"
#include "areg/base/RemoteMessage.hpp"
#include "areg/component/NEService.hpp"

ClientConnection::ClientConnection( void )
    : SocketConnectionBase    ( )
    , mClientSocket ( )
    , mCookie       ( NEService::COOKIE_UNKNOWN )
{
}

ClientConnection::ClientConnection(const String & hostName, unsigned short portNr)
    : SocketConnectionBase    ( )
    , mClientSocket ( hostName, portNr )
    , mCookie       ( NEService::COOKIE_UNKNOWN )
{
}

ClientConnection::ClientConnection(const NESocket::SocketAddress & remoteAddress)
    : SocketConnectionBase    ( )
    , mClientSocket ( remoteAddress )
    , mCookie       ( NEService::COOKIE_UNKNOWN )
{
}


bool ClientConnection::createSocket(const String & hostName, unsigned short portNr)
{
    setCookie( mClientSocket.createSocket(hostName, portNr) ? NEService::COOKIE_LOCAL : NEService::COOKIE_UNKNOWN );
    return mClientSocket.isValid();
}

bool ClientConnection::createSocket(void)
{
    setCookie( mClientSocket.createSocket() ? NEService::COOKIE_LOCAL : NEService::COOKIE_UNKNOWN );
    return mClientSocket.isValid();
}

void ClientConnection::closeSocket(void)
{
    setCookie(NEService::COOKIE_UNKNOWN);
    mClientSocket.closeSocket();
}

bool ClientConnection::requestConnectServer(void)
{
    bool result = false;
    if ( isValid() )
    {
        if ( getCookie() == NEService::COOKIE_LOCAL )
        {
            RemoteMessage msgHelloServer = NEConnection::createConnectRequest();
            result = msgHelloServer.isValid() ? sendMessage(msgHelloServer) > 0 : false;
        }
        else
        {
            result = true;  // nothing to set, valid connection
        }
    }
    return result;
}

bool ClientConnection::requestDisconnectServer(void)
{
    bool result = false;
    if ( isValid() )
    {
        RemoteMessage msgBeyServer = NEConnection::createDisconnectRequest(getCookie());
        result = msgBeyServer.isValid() ? sendMessage(msgBeyServer) > 0 : false;
        closeSocket();
    }
    return result;
}

RemoteMessage ClientConnection::getDisconnectMessage(void) const
{
    return (isValid( ) ? NEConnection::createDisconnectRequest( getCookie( ) ) : RemoteMessage( ));
}
