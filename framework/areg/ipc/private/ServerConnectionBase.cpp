/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/ipc/private/ServerConnectionBase.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform Server Connection class declaration.
 ************************************************************************/
#include "areg/ipc/ServerConnectionBase.hpp"

#include "areg/component/ServiceDefs.hpp"

ServerConnectionBase::ServerConnectionBase()
    : mServerSocket         ( )
    , mCookieGenerator      ( NEService::COOKIE_REMOTE_SERVICE )
    , mAcceptedConnections  ( )
    , mCookieToSocket       ( )
    , mSocketToCookie       ( )
    , mMasterList           ( ServerConnectionBase::MASTER_LIST_SIZE )
    , mLock                 ( )
{
}

ServerConnectionBase::ServerConnectionBase(const String & hostName, uint16_t portNr)
    : mServerSocket         ( hostName, portNr )
    , mCookieGenerator      ( NEService::COOKIE_REMOTE_SERVICE )
    , mAcceptedConnections  ( )
    , mCookieToSocket       ( )
    , mSocketToCookie       ( )
    , mMasterList           ( ServerConnectionBase::MASTER_LIST_SIZE )
    , mLock                 ( )
{
}

ServerConnectionBase::ServerConnectionBase(const NESocket::SocketAddress & serverAddress)
    : mServerSocket         ( serverAddress )
    , mCookieGenerator      ( NEService::COOKIE_REMOTE_SERVICE )
    , mAcceptedConnections  ( )
    , mCookieToSocket       ( )
    , mSocketToCookie       ( )
    , mMasterList           ( ServerConnectionBase::MASTER_LIST_SIZE )
    , mLock                 ( )
{
}

bool ServerConnectionBase::create_socket(const String & hostName, uint16_t portNr)
{
    Lock lock(mLock);
    return mServerSocket.create_socket(hostName, portNr);
}

bool ServerConnectionBase::create_socket()
{
    Lock lock(mLock);
    return mServerSocket.create_socket();
}

void ServerConnectionBase::close_socket()
{
    Lock lock(mLock);
    mMasterList.clear();
    mCookieToSocket.clear();
    mSocketToCookie.clear();
    mAcceptedConnections.clear();
    mCookieGenerator = NEService::COOKIE_REMOTE_SERVICE;

    mServerSocket.close_socket();
}

bool ServerConnectionBase::server_listen(int32_t maxQueueSize /*= NESocket::MAXIMUM_LISTEN_QUEUE_SIZE */)
{
    return mServerSocket.listen_connection(maxQueueSize);
}

SOCKETHANDLE ServerConnectionBase::wait_connection(NESocket::SocketAddress & out_addrNewAccepted)
{
    return mServerSocket.wait_connection_event(out_addrNewAccepted, static_cast<const SOCKETHANDLE *>(mMasterList), static_cast<int32_t>(mMasterList.size()));
}

bool ServerConnectionBase::accept_connection(SocketAccepted & clientConnection)
{
    Lock lock(mLock);
    bool result = false;

    if ( mServerSocket.is_valid() && clientConnection.is_valid( ) )
    {
        const SOCKETHANDLE hSocket = clientConnection.handle();
        ASSERT(hSocket != NESocket::InvalidSocketHandle);

        if ( mMasterList.find(hSocket) == -1)
        {
            ASSERT(mAcceptedConnections.contains( hSocket ) == false);
            ASSERT(mSocketToCookie.contains(hSocket) == false);

            ITEM_ID cookie{ mCookieGenerator ++ };
            ASSERT(cookie >= NEService::COOKIE_REMOTE_SERVICE);

            mAcceptedConnections.set_at(hSocket, clientConnection);
            mCookieToSocket.set_at(cookie, hSocket);
            mSocketToCookie.set_at(hSocket, cookie);
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

    return result;
}

void ServerConnectionBase::close_connection(SocketAccepted & clientConnection)
{
    Lock lock( mLock );

    SOCKETHANDLE hSocket{ clientConnection.handle() };
    MapSocketToCookie::MAPPOS pos{ mSocketToCookie.find(hSocket) };
    ITEM_ID cookie{ mSocketToCookie.is_valid_position(pos) ? static_cast<ITEM_ID>(mSocketToCookie.value_at_position(pos)) : NEService::COOKIE_UNKNOWN };

    mSocketToCookie.remove_at(hSocket);
    mCookieToSocket.remove_at(cookie);
    mAcceptedConnections.remove_at(hSocket);
    mMasterList.remove_elem(hSocket, 0);

    clientConnection.close_socket();
}

void ServerConnectionBase::close_connection( const ITEM_ID & cookie )
{
    Lock lock(mLock);

    MapCookieToSocket::MAPPOS posCookie = mCookieToSocket.find( cookie );
    if (mCookieToSocket.is_valid_position(posCookie))
    {
        SOCKETHANDLE hSocket = mCookieToSocket.value_at_position(posCookie);
        MapSocketToObject::MAPPOS posClient = mAcceptedConnections.find(hSocket);

        mCookieToSocket.remove_position( posCookie );        
        mSocketToCookie.remove_at( hSocket );
        mMasterList.remove_elem( hSocket, 0 );
        if (mAcceptedConnections.is_valid_position(posClient))
        {
            SocketAccepted client(mAcceptedConnections.value_at_position(posClient));
            mAcceptedConnections.remove_position(posClient);
            client.close_socket( );
        }
    }
}
