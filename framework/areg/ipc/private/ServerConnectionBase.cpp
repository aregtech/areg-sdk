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
namespace areg {

ServerConnectionBase::ServerConnectionBase()
    : mServerSocket         ( )
    , mMultiplexer          ( )
    , mCookieGenerator      ( areg::COOKIE_REMOTE_SERVICE )
    , mAcceptedConnections  ( )
    , mCookieToSocket       ( )
    , mSocketToCookie       ( )
    , mLock                 ( )
{
}

ServerConnectionBase::ServerConnectionBase(const String & hostName, uint16_t portNr)
    : mServerSocket         ( hostName, portNr )
    , mMultiplexer          ( )
    , mCookieGenerator      ( areg::COOKIE_REMOTE_SERVICE )
    , mAcceptedConnections  ( )
    , mCookieToSocket       ( )
    , mSocketToCookie       ( )
    , mLock                 ( )
{
}

ServerConnectionBase::ServerConnectionBase(const areg::SocketAddress & serverAddress)
    : mServerSocket         ( serverAddress )
    , mMultiplexer          ( )
    , mCookieGenerator      ( areg::COOKIE_REMOTE_SERVICE )
    , mAcceptedConnections  ( )
    , mCookieToSocket       ( )
    , mSocketToCookie       ( )
    , mLock                 ( )
{
}

bool ServerConnectionBase::create_socket(const String & hostName, uint16_t portNr)
{
    Lock lock(mLock);
    if (mServerSocket.create(hostName, portNr))
    {
        mMultiplexer.register_socket(mServerSocket.handle(), true);
        return true;
    }

    return false;
}

bool ServerConnectionBase::create_socket()
{
    Lock lock(mLock);
    if (mServerSocket.create())
    {
        mMultiplexer.register_socket(mServerSocket.handle(), true);
        return true;
    }

    return false;
}

void ServerConnectionBase::close_socket()
{
    Lock lock(mLock);
    // Reset multiplexer before closing sockets; on Linux, closed fds are
    // automatically removed from epoll, but reset() keeps the state consistent.
    mMultiplexer.reset();
    mCookieToSocket.clear();
    mSocketToCookie.clear();
    mAcceptedConnections.clear();
    mCookieGenerator = areg::COOKIE_REMOTE_SERVICE;

    mServerSocket.close();
}

bool ServerConnectionBase::server_listen(int32_t maxQueueSize /*= areg::MAXIMUM_LISTEN_QUEUE_SIZE */)
{
    return mServerSocket.listen(maxQueueSize);
}

SOCKETHANDLE ServerConnectionBase::wait_connection(areg::SocketAddress & out_addrNewAccepted)
{
    return mServerSocket.wait_connection_event(mMultiplexer, out_addrNewAccepted);
}

bool ServerConnectionBase::accept_connection(SocketAccepted & clientConnection)
{
    Lock lock(mLock);
    bool result = false;

    if ( mServerSocket.is_valid() && clientConnection.is_valid( ) )
    {
        const SOCKETHANDLE hSocket = clientConnection.handle();
        ASSERT(hSocket != areg::InvalidSocketHandle);
        ASSERT(mMultiplexer.is_registered(hSocket) == false);

        if ( !result && !mAcceptedConnections.contains(hSocket) )
        {
            ASSERT(mSocketToCookie.contains(hSocket) == false);

            // Register with the multiplexer before touching any state.
            // If the connection cap is reached, reject cleanly rather than
            // silently accepting a socket that will never be monitored.
            if (!mMultiplexer.register_socket(hSocket, false))
            {
                clientConnection.close();
                return false;
            }

            ITEM_ID cookie{ mCookieGenerator ++ };
            ASSERT(cookie >= areg::COOKIE_REMOTE_SERVICE);

            mAcceptedConnections.set_value_at(hSocket, clientConnection);
            mCookieToSocket.set_value_at(cookie, hSocket);
            mSocketToCookie.set_value_at(hSocket, cookie);
            result = true;
        }
        else
        {
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
    ITEM_ID cookie{ mSocketToCookie.is_valid_position(pos) ? static_cast<ITEM_ID>(mSocketToCookie.value_at(pos)) : areg::COOKIE_UNKNOWN };

    mMultiplexer.unregister_socket(hSocket);
    mSocketToCookie.remove_at(hSocket);
    mCookieToSocket.remove_at(cookie);
    mAcceptedConnections.remove_at(hSocket);

    clientConnection.close();
}

void ServerConnectionBase::close_connection( const ITEM_ID & cookie )
{
    Lock lock(mLock);

    MapCookieToSocket::MAPPOS posCookie = mCookieToSocket.find( cookie );
    if (mCookieToSocket.is_valid_position(posCookie))
    {
        SOCKETHANDLE hSocket = mCookieToSocket.value_at(posCookie);
        MapSocketToObject::MAPPOS posClient = mAcceptedConnections.find(hSocket);

        mMultiplexer.unregister_socket(hSocket);
        mCookieToSocket.remove_at( posCookie );
        mSocketToCookie.remove_at( hSocket );
        if (mAcceptedConnections.is_valid_position(posClient))
        {
            SocketAccepted client(mAcceptedConnections.value_at(posClient));
            mAcceptedConnections.remove_at(posClient);
            client.close();
        }
    }
}

} // namespace areg
