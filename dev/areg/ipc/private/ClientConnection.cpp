/************************************************************************
 * \file        areg/ipc/ClientConnection.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform Client Connection class declaration
 ************************************************************************/
#include "areg/ipc/private/ClientConnection.hpp"

#include "areg/ipc/private/NEConnection.hpp"
#include "areg/base/RemoteMessage.hpp"
#include "areg/component/NEService.hpp"

ClientConnection::ClientConnection( void )
    : SocketConnectionBase    ( )
    , mClientSocket ( )
    , mCookie       ( NEService::COOKIE_UNKNOWN )
{
    ; // do nothing
}

ClientConnection::ClientConnection(const char * hostName, unsigned int portNr)
    : SocketConnectionBase    ( )
    , mClientSocket ( hostName, portNr )
    , mCookie       ( NEService::COOKIE_UNKNOWN )
{
    ; // do nothing
}

ClientConnection::ClientConnection(const NESocket::InterlockedValue & remoteAddress)
    : SocketConnectionBase    ( )
    , mClientSocket ( remoteAddress )
    , mCookie       ( NEService::COOKIE_UNKNOWN )
{
    ; // do nothing
}

ClientConnection::~ClientConnection( void )
{
    ; // do nothing
}

bool ClientConnection::createSocket(const char * hostName, unsigned short portNr)
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
    return mClientSocket.closeSocket();
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
    RemoteMessage result;
    if (isValid())
    {
        result = NEConnection::createDisconnectRequest(getCookie());
    }

    return result;
}
