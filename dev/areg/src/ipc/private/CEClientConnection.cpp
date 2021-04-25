/************************************************************************
 * \file        areg/src/ipc/CEClientConnection.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform Client Connection class declaration
 ************************************************************************/
#include "areg/src/ipc/private/CEClientConnection.hpp"

#include "areg/src/ipc/private/NEConnection.hpp"
#include "areg/src/base/CERemoteMessage.hpp"
#include "areg/src/component/NEService.hpp"

CEClientConnection::CEClientConnection( void )
    : CESocketConnectionBase    ( )
    , mClientSocket ( )
    , mCookie       ( NEService::COOKIE_UNKNOWN )
{
    ; // do nothing
}

CEClientConnection::CEClientConnection(const char * hostName, unsigned int portNr)
    : CESocketConnectionBase    ( )
    , mClientSocket ( hostName, portNr )
    , mCookie       ( NEService::COOKIE_UNKNOWN )
{
    ; // do nothing
}

CEClientConnection::CEClientConnection(const NESocket::CEInterlockedValue & remoteAddress)
    : CESocketConnectionBase    ( )
    , mClientSocket ( remoteAddress )
    , mCookie       ( NEService::COOKIE_UNKNOWN )
{
    ; // do nothing
}

CEClientConnection::~CEClientConnection( void )
{
    ; // do nothing
}

bool CEClientConnection::CreateSocket(const char * hostName, unsigned short portNr)
{
    SetCookie( mClientSocket.CreateSocket(hostName, portNr) ? NEService::COOKIE_LOCAL : NEService::COOKIE_UNKNOWN );
    return mClientSocket.IsValid();
}

bool CEClientConnection::CreateSocket(void)
{
    SetCookie( mClientSocket.CreateSocket() ? NEService::COOKIE_LOCAL : NEService::COOKIE_UNKNOWN );
    return mClientSocket.IsValid();
}

inline void CEClientConnection::CloseSocket(void)
{
    SetCookie(NEService::COOKIE_UNKNOWN);
    return mClientSocket.CloseSocket();
}

bool CEClientConnection::RequestConnectServer(void)
{
    bool result = false;
    if ( IsValid() )
    {
        if ( GetCookie() == NEService::COOKIE_LOCAL )
        {
            CERemoteMessage msgHelloServer = NEConnection::CreateConnectRequest();
            result = msgHelloServer.IsValid() ? SendMessage(msgHelloServer) > 0 : false;
        }
        else
        {
            result = true;  // nothing to set, valid connection
        }
    }
    return result;
}

bool CEClientConnection::RequestDisconnectServer(void)
{
    bool result = false;
    if ( IsValid() )
    {
        CERemoteMessage msgBeyServer = NEConnection::CreateDisconnectRequest(GetCookie());
        result = msgBeyServer.IsValid() ? SendMessage(msgBeyServer) > 0 : false;
        CloseSocket();
    }
    return result;
}
