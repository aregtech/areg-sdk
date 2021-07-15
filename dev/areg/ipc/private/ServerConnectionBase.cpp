/************************************************************************
 * \file        areg/ipc/private/ServerConnectionBase.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform Server Connection class declaration.
 ************************************************************************/
#include "areg/ipc/ServerConnectionBase.hpp"

const int       ServerConnectionBase::MASTER_LIST_SIZE    = 64;

ServerConnectionBase::ServerConnectionBase( void )
    : mServerSocket         ( )
    , mCookieGenerator      ( static_cast<ITEM_ID>(NEService::CookieFirstValid) )
    , mAcceptedConnections  ( )
    , mCookieToSocket       ( )
    , mSocketToCookie       ( )
    , mMasterList           ( ServerConnectionBase::MASTER_LIST_SIZE )
    , mLock                 ( )
{
}

ServerConnectionBase::ServerConnectionBase(const char * hostName, unsigned short portNr)
    : mServerSocket         ( hostName, portNr )
    , mCookieGenerator      ( static_cast<ITEM_ID>(NEService::CookieFirstValid) )
    , mAcceptedConnections  ( )
    , mCookieToSocket       ( )
    , mSocketToCookie       ( )
    , mMasterList           ( ServerConnectionBase::MASTER_LIST_SIZE )
    , mLock                 ( )
{
}

ServerConnectionBase::ServerConnectionBase(const NESocket::InterlockedValue & serverAddress)
    : mServerSocket         ( serverAddress )
    , mCookieGenerator      ( static_cast<ITEM_ID>(NEService::CookieFirstValid) )
    , mAcceptedConnections  ( )
    , mCookieToSocket       ( )
    , mSocketToCookie       ( )
    , mMasterList           ( ServerConnectionBase::MASTER_LIST_SIZE )
    , mLock                 ( )
{
}

ServerConnectionBase::~ServerConnectionBase(void)
{
}

bool ServerConnectionBase::createSocket(const char * hostName, unsigned short portNr)
{
    Lock lock(mLock);
    return mServerSocket.createSocket(hostName, portNr);
}

bool ServerConnectionBase::createSocket(void)
{
    Lock lock(mLock);
    return mServerSocket.createSocket();
}

void ServerConnectionBase::closeSocket(void)
{
    Lock lock(mLock);
    mMasterList.removeAll();
    mCookieToSocket.removeAll();
    mSocketToCookie.removeAll();
    mAcceptedConnections.removeAll();
    mCookieGenerator = static_cast<ITEM_ID>(NEService::CookieFirstValid);

    mServerSocket.closeSocket();
}

bool ServerConnectionBase::serverListen(int maxQueueSize /*= NESocket::MAXIMUM_LISTEN_QUEUE_SIZE */)
{
    return mServerSocket.listenConnection(maxQueueSize);
}

SOCKETHANDLE ServerConnectionBase::waitForConnectionEvent(NESocket::InterlockedValue & out_addrNewAccepted)
{
    return mServerSocket.waitConnectionEvent(out_addrNewAccepted, static_cast<const SOCKETHANDLE *>(mMasterList), mMasterList.getSize());
}

bool ServerConnectionBase::acceptConnection(SocketAccepted & clientConnection)
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

void ServerConnectionBase::closeConnection(SocketAccepted & clientConnection)
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

void ServerConnectionBase::closeConnection( ITEM_ID cookie )
{
    Lock lock(mLock);

    MAPPOS posCookie = mCookieToSocket.find( cookie );
    if (posCookie != NULL)
    {
        SOCKETHANDLE hSocket= mCookieToSocket.removePosition( posCookie );
        MAPPOS posClient    = mAcceptedConnections.find( hSocket );
        mSocketToCookie.removeAt( hSocket );
        mMasterList.remove( hSocket, 0 );
        if (posClient != NULL)
        {
            SocketAccepted client(mAcceptedConnections.removePosition(posClient));
            client.closeSocket( );
        }
    }
}
