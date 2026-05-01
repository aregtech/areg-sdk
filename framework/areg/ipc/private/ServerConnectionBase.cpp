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

#include "areg/base/SocketDefs.hpp"
#include "areg/component/ServiceDefs.hpp"
#include "areg/logging/areg_log.h"

namespace areg {

DEF_LOG_SCOPE(areg_ipc_ServerConnectionBase, accept_connection);
DEF_LOG_SCOPE(areg_ipc_ServerConnectionBase, close_connection_cookie);

ServerConnectionBase::ServerConnectionBase()
    : mServerSocket         ( )
    , mMultiplexer          ( )
    , mCookieGenerator      ( areg::COOKIE_REMOTE_SERVICE )
    , mAcceptedConnections  ( )
    , mCookieToSocket       ( )
    , mSocketToCookie       ( )
    , mSockSendBuf          ( areg::SOCKET_SEND_BUFFER_SIZE )
    , mSockRecvBuf          ( areg::SOCKET_RECV_BUFFER_SIZE )
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
    , mSockSendBuf          ( areg::SOCKET_SEND_BUFFER_SIZE )
    , mSockRecvBuf          ( areg::SOCKET_RECV_BUFFER_SIZE )
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
    , mSockSendBuf          ( areg::SOCKET_SEND_BUFFER_SIZE )
    , mSockRecvBuf          ( areg::SOCKET_RECV_BUFFER_SIZE )
    , mLock                 ( )
{
}

bool ServerConnectionBase::create_socket(const String & hostName, uint16_t portNr)
{
    std::unique_lock<std::shared_mutex> lock(mLock);
    if (mServerSocket.create(hostName, portNr))
    {
        mMultiplexer.register_socket(mServerSocket.handle(), true);
        mIsInterrupted.store(false, std::memory_order_release);
        return true;
    }

    return false;
}

bool ServerConnectionBase::create_socket()
{
    std::unique_lock<std::shared_mutex> lock(mLock);
    if (mServerSocket.create())
    {
        mMultiplexer.register_socket(mServerSocket.handle(), true);
        mIsInterrupted.store(false, std::memory_order_release);
        return true;
    }

    return false;
}

void ServerConnectionBase::close_socket()
{
    std::unique_lock<std::shared_mutex> lock(mLock);

    // Reset the multiplexer first so that epoll_wait() / select() in the
    // receive thread unblocks immediately (signals the internal wakeup fd).
    mMultiplexer.reset();

    // Interrupt every accepted client socket so that any thread currently
    // blocked in send() or recv() on that fd returns with an error.
    for ( MapSocketToObject::MAPPOS pos = mAcceptedConnections.first_position();
          mAcceptedConnections.is_valid_position(pos);
          pos = mAcceptedConnections.next_position(pos) )
    {
        areg::socket_interrupt(mAcceptedConnections.value_at(pos).handle());
    }

    mCookieToSocket.clear();
    mSocketToCookie.clear();
    mAcceptedConnections.clear();
    mCookieGenerator = areg::COOKIE_REMOTE_SERVICE;

    mServerSocket.close();
}

void ServerConnectionBase::interrupt_connections() noexcept
{
    {
        std::shared_lock<std::shared_mutex> lock(mLock);
        for ( MapSocketToObject::MAPPOS pos = mAcceptedConnections.first_position();
              mAcceptedConnections.is_valid_position(pos);
              pos = mAcceptedConnections.next_position(pos) )
        {
            areg::socket_interrupt(mAcceptedConnections.value_at(pos).handle());
        }
    }

    mIsInterrupted.store(true, std::memory_order_release);
}

bool ServerConnectionBase::server_listen(int32_t maxQueueSize /*= areg::MAXIMUM_LISTEN_QUEUE_SIZE */)
{
    return mServerSocket.listen(maxQueueSize);
}

SOCKETHANDLE ServerConnectionBase::wait_connection(areg::SocketAddress & out_addrNewAccepted)
{
    return mServerSocket.wait_connection_event(mMultiplexer, out_addrNewAccepted);
}

SOCKETHANDLE ServerConnectionBase::wait_connection_nowait(areg::SocketAddress & out_addrNewAccepted)
{
    return mServerSocket.wait_connection_nowait(mMultiplexer, out_addrNewAccepted);
}

bool ServerConnectionBase::accept_connection(SocketAccepted & clientConnection)
{
    LOG_SCOPE(areg_ipc_ServerConnectionBase, accept_connection);
    std::unique_lock<std::shared_mutex> lock(mLock);
    bool result = false;

    if ( mServerSocket.is_valid() && clientConnection.is_valid( ) )
    {
        const SOCKETHANDLE hSocket = clientConnection.handle();
        ASSERT(hSocket != areg::InvalidSocketHandle);
        ASSERT(mMultiplexer.is_registered(hSocket) == false);

        if ( !result && !mAcceptedConnections.contains(hSocket) )
        {
            ASSERT(mSocketToCookie.contains(hSocket) == false);

            ASSERT(!mMultiplexer.is_registered(hSocket));
            if (!mMultiplexer.register_socket(hSocket, false))
            {
                clientConnection.close();
                return false;
            }

            // SO_SNDBUF: applied on all platforms.  Setting SO_SNDBUF on Windows does NOT
            // disable TCP Send Window autotuning — only SO_RCVBUF has that side-effect.
            areg::set_send_size(hSocket, mSockSendBuf);

            // SO_RCVBUF: skipped on Windows because setsockopt(SO_RCVBUF) disables TCP
            // Receive Window Autotuning (Vista+), which dynamically tunes better than any
            // fixed value for loopback and LAN connections.  Linux/macOS need explicit
            // sizing to overcome the conservative kernel defaults.
#if !defined(_WIN32)
            areg::set_recv_size(hSocket, mSockRecvBuf);
#endif  // !defined(_WIN32)

            // SO_SNDTIMEO: kernel-level send timeout.  If _os_send_data's
            // application-level deadline is somehow bypassed (e.g. send()
            // blocks despite MSG_DONTWAIT), this ensures the kernel returns
            // EAGAIN after SOCKET_SEND_TIMEOUT_MS.  Harmless on non-blocking
            // sockets — the flag has no effect when send() returns immediately.
            areg::set_send_timeout(hSocket, areg::SOCKET_SEND_TIMEOUT_MS);

            ITEM_ID cookie{ mCookieGenerator ++ };
            ASSERT(cookie >= areg::COOKIE_REMOTE_SERVICE);

            mAcceptedConnections.set_value_at(hSocket, clientConnection);
            mCookieToSocket.set_value_at(cookie, hSocket);
            mSocketToCookie.set_value_at(hSocket, cookie);
            result = true;

            LOG_INFO("Accepted new connection: socket [ %u ] assigned cookie [ %u ], total accepted [ %u ]"
                        , static_cast<uint32_t>(hSocket)
                        , static_cast<uint32_t>(cookie)
                        , mAcceptedConnections.size());
        }
        else
        {
            ASSERT(mSocketToCookie.contains(hSocket));
            LOG_WARN("Connection on socket [ %u ] already accepted (duplicate accept call), total accepted [ %u ]"
                        , static_cast<uint32_t>(hSocket)
                        , mAcceptedConnections.size());
            result = true;
        }
    }

    return result;
}

void ServerConnectionBase::close_connection(SocketAccepted & clientConnection)
{
    std::unique_lock<std::shared_mutex> lock( mLock );

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
    LOG_SCOPE(areg_ipc_ServerConnectionBase, close_connection_cookie);
    std::unique_lock<std::shared_mutex> lock(mLock);

    MapCookieToSocket::MAPPOS posCookie = mCookieToSocket.find( cookie );
    if (mCookieToSocket.is_valid_position(posCookie))
    {
        SOCKETHANDLE hSocket = mCookieToSocket.value_at(posCookie);
        MapSocketToObject::MAPPOS posClient = mAcceptedConnections.find(hSocket);

        mMultiplexer.unregister_socket(hSocket);
        // Shut down I/O immediately.
        areg::socket_interrupt(hSocket);
        mCookieToSocket.remove_at( posCookie );
        mSocketToCookie.remove_at( hSocket );
        if (mAcceptedConnections.is_valid_position(posClient))
        {
            SocketAccepted client(mAcceptedConnections.value_at(posClient));
            mAcceptedConnections.remove_at(posClient);
            client.close();
        }
        else
        {
            // The socket fd is not owned by any SocketAccepted, close it directly.
            areg::socket_close(hSocket);
        }

        LOG_INFO("Closed connection for cookie [ %u ], socket [ %u ], remaining accepted [ %u ]"
                    , static_cast<uint32_t>(cookie)
                    , static_cast<uint32_t>(hSocket)
                    , mAcceptedConnections.size());
    }
    else
    {
        LOG_WARN("close_connection called for unknown cookie [ %u ] — already closed or never registered"
                    , static_cast<uint32_t>(cookie));
    }
}

} // namespace areg
