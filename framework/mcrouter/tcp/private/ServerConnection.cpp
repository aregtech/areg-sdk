/************************************************************************
 * \file        mcrouter/tcp/private/ServerConnection.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform Server Connection class declaration.
 ************************************************************************/
#include "mcrouter/tcp/private/ServerConnection.hpp"
#include "areg/ipc/NEConnection.hpp"
#include "areg/component/NEService.hpp"
#include "areg/base/RemoteMessage.hpp"

ServerConnection::ServerConnection( void )
    : ServerConnectionBase  ( )
    , SocketConnectionBase  ( )
{
}

ServerConnection::ServerConnection(const char * hostName, unsigned short portNr)
    : ServerConnectionBase  ( )
    , SocketConnectionBase  ( )
{
}

ServerConnection::ServerConnection(const NESocket::InterlockedValue & serverAddress)
    : ServerConnectionBase  ( )
    , SocketConnectionBase  ( )
{
}

ServerConnection::~ServerConnection(void)
{
}

void ServerConnection::rejectConnection(SocketAccepted & clientConnection)
{
    ITEM_ID cookie = getCookie(clientConnection.getHandle());
    RemoteMessage msgReject = NEConnection::createRejectNotify(cookie);
    sendMessage(msgReject, clientConnection);
    closeConnection(clientConnection);
}

void ServerConnection::closeAllConnections(void)
{
    Lock lock( mLock );
    RemoteMessage msgBeyClient;
    if ( msgBeyClient.initMessage( NEConnection::MessageByeClient.rbHeader ) != NULL )
    {
        msgBeyClient.setSequenceNr( NEService::SEQUENCE_NUMBER_ANY );
        msgBeyClient.setSource( NEService::COOKIE_ROUTER );
        msgBeyClient.bufferCompletionFix();
    }

    for ( MAPPOS pos = mAcceptedConnections.firstPosition(); pos != NULL; pos = mAcceptedConnections.nextPosition(pos) )
    {
        SocketAccepted clientConnection = mAcceptedConnections.valueAtPosition(pos);
        msgBeyClient.setTarget( getCookie(clientConnection) );
        sendMessage(msgBeyClient, clientConnection);
    }

    mMasterList.removeAll();
    mCookieToSocket.removeAll();
    mSocketToCookie.removeAll();
    mAcceptedConnections.removeAll();

    mCookieGenerator    = static_cast<ITEM_ID>(NEService::CookieFirstValid);
}
