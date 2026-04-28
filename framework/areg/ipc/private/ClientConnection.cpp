/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/ipc/private/ClientConnection.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform Client Connection class declaration
 ************************************************************************/
#include "areg/ipc/ClientConnection.hpp"

#include "areg/base/RemoteMessage.hpp"
#include "areg/base/SocketDefs.hpp"
#include "areg/component/ServiceDefs.hpp"

namespace areg {

ClientConnection::ClientConnection()
    : SocketConnectionBase    ( )
    , mClientSocket ( )
    , mCookie       ( areg::COOKIE_UNKNOWN )
    , mSockSendBuf  ( areg::SOCKET_SEND_BUFFER_SIZE )
    , mSockRecvBuf  ( areg::SOCKET_RECV_BUFFER_SIZE )
{
}

ClientConnection::ClientConnection(const String & hostName, uint16_t portNr)
    : SocketConnectionBase    ( )
    , mClientSocket ( hostName, portNr )
    , mCookie       ( areg::COOKIE_UNKNOWN )
    , mSockSendBuf  ( areg::SOCKET_SEND_BUFFER_SIZE )
    , mSockRecvBuf  ( areg::SOCKET_RECV_BUFFER_SIZE )
{
}

ClientConnection::ClientConnection(const areg::SocketAddress & remoteAddress)
    : SocketConnectionBase    ( )
    , mClientSocket ( remoteAddress )
    , mCookie       ( areg::COOKIE_UNKNOWN )
    , mSockSendBuf  ( areg::SOCKET_SEND_BUFFER_SIZE )
    , mSockRecvBuf  ( areg::SOCKET_RECV_BUFFER_SIZE )
{
}


bool ClientConnection::create_socket(const String & hostName, uint16_t portNr)
{
    set_cookie( mClientSocket.create(hostName, portNr) ? areg::COOKIE_LOCAL : areg::COOKIE_UNKNOWN );
    if (mClientSocket.is_valid())
    {
        // SO_SNDBUF: applied on all platforms.  Setting SO_SNDBUF on Windows does NOT
        // disable TCP Send Window autotuning — only SO_RCVBUF has that side-effect.
        // A large send buffer (default 16 MB) keeps many large frames in-flight and
        // prevents TCP stalls on high-throughput image pipelines.
        areg::set_send_size(mClientSocket.handle(), mSockSendBuf);

        // SO_RCVBUF: skip on Windows.  setsockopt(SO_RCVBUF) disables TCP Receive
        // Window Autotuning (Vista+), which is more effective than any fixed value
        // for loopback and LAN.
#if !defined(_WIN32)
        areg::set_recv_size(mClientSocket.handle(), mSockRecvBuf);
#endif  // !defined(_WIN32)

        // SO_SNDTIMEO ensures blocking send() returns after the timeout instead
        // of blocking indefinitely when the peer is unresponsive.
        areg::set_send_timeout(mClientSocket.handle(), areg::SOCKET_SEND_TIMEOUT_MS);

#if defined(__linux__)
        mZerocopyEnabled = mZerocopyWanted && areg::socket_enable_zerocopy(mClientSocket.handle());
#endif  // defined(__linux__)
    }

    return mClientSocket.is_valid();
}

bool ClientConnection::create_socket()
{
    set_cookie( mClientSocket.create() ? areg::COOKIE_LOCAL : areg::COOKIE_UNKNOWN );
    if (mClientSocket.is_valid())
    {
        // SO_SNDBUF: applied on all platforms (see comment in create_socket(host, port) above).
        areg::set_send_size(mClientSocket.handle(), mSockSendBuf);

#if !defined(_WIN32)
        areg::set_recv_size(mClientSocket.handle(), mSockRecvBuf);
#endif  // !defined(_WIN32)

        areg::set_send_timeout(mClientSocket.handle(), areg::SOCKET_SEND_TIMEOUT_MS);

#if defined(__linux__)
        mZerocopyEnabled = mZerocopyWanted && areg::socket_enable_zerocopy(mClientSocket.handle());
#endif  // defined(__linux__)
    }

    return mClientSocket.is_valid();
}

void ClientConnection::close_socket()
{
    set_cookie(areg::COOKIE_UNKNOWN);
#if defined(__linux__)
    mZerocopyEnabled = false;
#endif  // defined(__linux__)
    mClientSocket.close();
}

#if defined(__linux__)

int32_t ClientConnection::send_message_zerocopy(const RemoteMessage& in_message) const
{
    return SocketConnectionBase::send_message_zerocopy(in_message, mClientSocket.handle());
}

#endif  // defined(__linux__)

} // namespace areg
