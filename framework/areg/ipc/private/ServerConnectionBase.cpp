/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/ipc/private/ServerConnectionBase.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform Server Connection class declaration.
 ************************************************************************/
#include "areg/ipc/ServerConnectionBase.hpp"

ServerConnectionBase::ServerConnectionBase( void )
    : mServerSocket         ( )
    , mCookieGenerator      ( static_cast<ITEM_ID>(NEService::eCookies::CookieFirstValid) )
    , mAcceptedConnections  ( )
    , mCookieToSocket       ( )
    , mSocketToCookie       ( )
    , mMasterList           ( ServerConnectionBase::MASTER_LIST_SIZE )
    , mLock                 ( )
{
}

ServerConnectionBase::ServerConnectionBase(const char * hostName, unsigned short portNr)
    : mServerSocket         ( hostName, portNr )
    , mCookieGenerator      ( static_cast<ITEM_ID>(NEService::eCookies::CookieFirstValid) )
    , mAcceptedConnections  ( )
    , mCookieToSocket       ( )
    , mSocketToCookie       ( )
    , mMasterList           ( ServerConnectionBase::MASTER_LIST_SIZE )
    , mLock                 ( )
{
}

ServerConnectionBase::ServerConnectionBase(const NESocket::SocketAddress & serverAddress)
    : mServerSocket         ( serverAddress )
    , mCookieGenerator      ( static_cast<ITEM_ID>(NEService::eCookies::CookieFirstValid) )
    , mAcceptedConnections  ( )
    , mCookieToSocket       ( )
    , mSocketToCookie       ( )
    , mMasterList           ( ServerConnectionBase::MASTER_LIST_SIZE )
    , mLock                 ( )
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
    mCookieGenerator = static_cast<ITEM_ID>(NEService::eCookies::CookieFirstValid);

    mServerSocket.closeSocket();
}

bool ServerConnectionBase::serverListen(int maxQueueSize /*= NESocket::MAXIMUM_LISTEN_QUEUE_SIZE */)
{
    return mServerSocket.listenConnection(maxQueueSize);
}

SOCKETHANDLE ServerConnectionBase::waitForConnectionEvent(NESocket::SocketAddress & out_addrNewAccepted)
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
                ASSERT(mAcceptedConnections.find( hSocket ) == nullptr);
                ASSERT(mSocketToCookie.find(hSocket) == nullptr);

                ITEM_ID cookie = mCookieGenerator ++;
                ASSERT(cookie >= static_cast<ITEM_ID>(NEService::eCookies::CookieFirstValid));

                mAcceptedConnections.setAt(hSocket, clientConnection);
                mCookieToSocket.setAt(cookie, hSocket);
                mSocketToCookie.setAt(hSocket, cookie);
                mMasterList.add( hSocket );
                result = true;
            }
            else
            {
                ASSERT(mAcceptedConnections.find( hSocket ) != nullptr);
                ASSERT(mSocketToCookie.find(hSocket) != nullptr);
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
    ITEM_ID cookie = pos != nullptr ? mSocketToCookie.valueAtPosition(pos) : NEService::COOKIE_UNKNOWN;

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
    if (posCookie != nullptr)
    {
        SOCKETHANDLE hSocket= mCookieToSocket.removePosition( posCookie );
        MAPPOS posClient    = mAcceptedConnections.find( hSocket );
        mSocketToCookie.removeAt( hSocket );
        mMasterList.remove( hSocket, 0 );
        if (posClient != nullptr)
        {
            SocketAccepted client(mAcceptedConnections.removePosition(posClient));
            client.closeSocket( );
        }
    }
}
