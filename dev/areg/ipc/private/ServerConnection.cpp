/************************************************************************
 * \file        areg/ipc/private/ServerConnection.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform Server Connection class declaration.
 ************************************************************************/
#include "areg/ipc/private/ServerConnection.hpp"
#include "areg/ipc/private/NEConnection.hpp"
#include "areg/component/NEService.hpp"
#include "areg/base/RemoteMessage.hpp"

const int       ServerConnection::MASTER_LIST_SIZE    = 64;

ServerConnection::ServerConnection( void )
    : SocketConnectionBase( )
    , mServerSocket         ( )
    , mCookieGenerator      ( static_cast<ITEM_ID>(NEService::CookieFirstValid) )
    , mAcceptedConnections  ( )
    , mCookieToSocket       ( )
    , mSocketToCookie       ( )
    , mMasterList           ( ServerConnection::MASTER_LIST_SIZE )
    , mLock                 ( )
{
    ; // do nothing
}

ServerConnection::ServerConnection(const char * hostName, unsigned short portNr)
    : SocketConnectionBase( )
    , mServerSocket         ( hostName, portNr )
    , mCookieGenerator      ( static_cast<ITEM_ID>(NEService::CookieFirstValid) )
    , mAcceptedConnections  ( )
    , mCookieToSocket       ( )
    , mSocketToCookie       ( )
    , mMasterList           ( ServerConnection::MASTER_LIST_SIZE )
    , mLock                 ( )
{
    ; // do nothing
}

ServerConnection::ServerConnection(const NESocket::InterlockedValue & serverAddress)
    : SocketConnectionBase( )
    , mServerSocket         ( serverAddress )
    , mCookieGenerator      ( static_cast<ITEM_ID>(NEService::CookieFirstValid) )
    , mAcceptedConnections  ( )
    , mCookieToSocket       ( )
    , mSocketToCookie       ( )
    , mMasterList           ( ServerConnection::MASTER_LIST_SIZE )
    , mLock                 ( )
{
    ; // do nothing
}

ServerConnection::~ServerConnection(void)
{
    ; // do nothing
}

bool ServerConnection::createSocket(const char * hostName, unsigned short portNr)
{
    Lock lock(mLock);
    return mServerSocket.createSocket(hostName, portNr);
}

bool ServerConnection::createSocket(void)
{
    Lock lock(mLock);
    return mServerSocket.createSocket();
}

void ServerConnection::closeSocket(void)
{
    Lock lock(mLock);
    mMasterList.removeAll();
    mCookieToSocket.removeAll();
    mSocketToCookie.removeAll();
    mAcceptedConnections.removeAll();
    mCookieGenerator = static_cast<ITEM_ID>(NEService::CookieFirstValid);

    mServerSocket.closeSocket();
}

bool ServerConnection::serverListen(int maxQueueSize /*= NESocket::MAXIMUM_LISTEN_QUEUE_SIZE */)
{
    return mServerSocket.listenConnection(maxQueueSize);
}

SOCKETHANDLE ServerConnection::waitForConnectionEvent(NESocket::InterlockedValue & out_addrNewAccepted)
{
    return mServerSocket.waitConnectionEvent(out_addrNewAccepted, static_cast<const SOCKETHANDLE *>(mMasterList), mMasterList.getSize());
}

bool ServerConnection::acceptConnection(SocketAccepted & clientConnection)
{
    Lock lock(mLock);
    bool result = false;

    if ( mServerSocket.isValid() )
    {
        if ( clientConnection.isValid() )
        {
            const SOCKETHANDLE hSocket = clientConnection.getHandle();
            ASSERT(hSocket != NESocket::InvalidSocketHandle);

            if ( mMasterList.find(hSocket) == -1)
            {
                ASSERT(mAcceptedConnections.find( hSocket ) == NULL);
                ASSERT(mSocketToCookie.find(hSocket) == NULL);

                ITEM_ID cookie = mCookieGenerator ++;
                ASSERT(cookie >= static_cast<ITEM_ID>(NEService::CookieFirstValid));

                mAcceptedConnections.setAt(hSocket, clientConnection);
                mCookieToSocket.setAt(cookie, hSocket);
                mSocketToCookie.setAt(hSocket, cookie);
                mMasterList.add( hSocket );
                result = true;
            }
            else
            {
                ASSERT(mAcceptedConnections.find( hSocket ) != NULL);
                ASSERT(mSocketToCookie.find(hSocket) != NULL);
                result = true;
            }
        }
    }

    return result;
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

ITEM_ID ServerConnection::getCookie(const SocketAccepted & clientSocket) const
{
    return getCookie(clientSocket.getHandle());
}

ITEM_ID ServerConnection::getCookie(SOCKETHANDLE socketHandle) const
{
    Lock lock( mLock );

    MAPPOS pos = mSocketToCookie.find( socketHandle );
    return ( pos != NULL ? mSocketToCookie.valueAtPosition(pos) : NEService::COOKIE_UNKNOWN );
}

SocketAccepted ServerConnection::getClientByCookie(ITEM_ID clientCookie) const
{
    Lock lock( mLock );

    SocketAccepted result;
    MAPPOS pos = mCookieToSocket.find(clientCookie);
    if ( pos != NULL )
        result = getClientByHandle( mCookieToSocket.valueAtPosition(pos) );

    return result;
}

SocketAccepted ServerConnection::getClientByHandle(SOCKETHANDLE clientSocket) const
{
    Lock lock( mLock );

    SocketAccepted result;
    MAPPOS pos = mAcceptedConnections.find(clientSocket);
    if ( pos != NULL )
        result = mAcceptedConnections.getAt(clientSocket);

    return result;
}


void ServerConnection::closeConnection(SocketAccepted & clientConnection)
{
    Lock lock( mLock );

    SOCKETHANDLE hSocket= clientConnection.getHandle();
    MAPPOS pos = mSocketToCookie.find(hSocket);
    ITEM_ID cookie = pos != NULL ? mSocketToCookie.valueAtPosition(pos) : NEService::COOKIE_UNKNOWN;

    mSocketToCookie.removeAt(hSocket);
    mCookieToSocket.removeAt(cookie);
    mAcceptedConnections.removeAt(hSocket);
    mMasterList.remove(hSocket, 0);

    clientConnection.closeSocket();
}
