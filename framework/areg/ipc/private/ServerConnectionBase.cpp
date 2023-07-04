/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/ipc/private/ServerConnectionBase.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform Server Connection class declaration.
 ************************************************************************/
#include "areg/ipc/ServerConnectionBase.hpp"

#include "areg/component/NEService.hpp"

ServerConnectionBase::ServerConnectionBase( void )
    : mServerSocket         ( )
    , mCookieGenerator      ( NEService::COOKIE_FIRST )
    , mAcceptedConnections  ( )
    , mCookieToSocket       ( )
    , mSocketToCookie       ( )
    , mMasterList           ( ServerConnectionBase::MASTER_LIST_SIZE )
    , mLock                 ( )
{
}

ServerConnectionBase::ServerConnectionBase(const String & hostName, unsigned short portNr)
    : mServerSocket         ( hostName, portNr )
    , mCookieGenerator      ( NEService::COOKIE_FIRST )
    , mAcceptedConnections  ( )
    , mCookieToSocket       ( )
    , mSocketToCookie       ( )
    , mMasterList           ( ServerConnectionBase::MASTER_LIST_SIZE )
    , mLock                 ( )
{
}

ServerConnectionBase::ServerConnectionBase(const NESocket::SocketAddress & serverAddress)
    : mServerSocket         ( serverAddress )
    , mCookieGenerator      ( NEService::COOKIE_FIRST )
    , mAcceptedConnections  ( )
    , mCookieToSocket       ( )
    , mSocketToCookie       ( )
    , mMasterList           ( ServerConnectionBase::MASTER_LIST_SIZE )
    , mLock                 ( )
{
}

bool ServerConnectionBase::createSocket(const String & hostName, unsigned short portNr)
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
    mMasterList.clear();
    mCookieToSocket.clear();
    mSocketToCookie.clear();
    mAcceptedConnections.clear();
    mCookieGenerator = NEService::COOKIE_FIRST;

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
                ASSERT(mAcceptedConnections.contains( hSocket ) == false);
                ASSERT(mSocketToCookie.contains(hSocket) == false);

                ITEM_ID cookie = mCookieGenerator ++;
                ASSERT(cookie >= NEService::COOKIE_FIRST);

                mAcceptedConnections.setAt(hSocket, clientConnection);
                mCookieToSocket.setAt(cookie, hSocket);
                mSocketToCookie.setAt(hSocket, cookie);
                mMasterList.add( hSocket );
                result = true;
            }
            else
            {
                ASSERT(mAcceptedConnections.contains( hSocket ));
                ASSERT(mSocketToCookie.contains(hSocket));
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
    MapSocketToCookie::MAPPOS pos = mSocketToCookie.find(hSocket);
    ITEM_ID cookie = mSocketToCookie.isValidPosition(pos) ? mSocketToCookie.valueAtPosition(pos) : NEService::COOKIE_UNKNOWN;

    mSocketToCookie.removeAt(hSocket);
    mCookieToSocket.removeAt(cookie);
    mAcceptedConnections.removeAt(hSocket);
    mMasterList.removeElem(hSocket, 0);

    clientConnection.closeSocket();
}

void ServerConnectionBase::closeConnection( ITEM_ID cookie )
{
    Lock lock(mLock);

    MapCookieToSocket::MAPPOS posCookie = mCookieToSocket.find( cookie );
    if (mCookieToSocket.isValidPosition(posCookie))
    {
        SOCKETHANDLE hSocket = mCookieToSocket.valueAtPosition(posCookie);
        MapSocketToObject::MAPPOS posClient = mAcceptedConnections.find(hSocket);

        mCookieToSocket.removePosition( posCookie );        
        mSocketToCookie.removeAt( hSocket );
        mMasterList.removeElem( hSocket, 0 );
        if (mAcceptedConnections.isValidPosition(posClient))
        {
            SocketAccepted client(mAcceptedConnections.valueAtPosition(posClient));
            mAcceptedConnections.removePosition(posClient);
            client.closeSocket( );
        }
    }
}
