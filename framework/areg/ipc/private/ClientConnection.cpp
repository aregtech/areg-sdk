/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        areg/ipc/private/ClientConnection.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform Client Connection class declaration
 ************************************************************************/
#include "areg/ipc/ClientConnection.hpp"

#include "areg/base/SocketDefs.hpp"
#include "areg/component/ServiceDefs.hpp"

namespace areg {

ClientConnection::ClientConnection()
    : SocketConnectionBase  ( )
    , mClientSocket         ( )
    , mCookie               ( areg::COOKIE_UNKNOWN )
    , mSockSendBuf          ( areg::SOCKET_SEND_BUFFER_SIZE )
    , mSockRecvBuf          ( areg::SOCKET_RECV_BUFFER_SIZE )
    , mSockSendTimeoutMs    ( areg::SOCKET_SEND_TIMEOUT_MS )
{
}

ClientConnection::ClientConnection(const String & hostName, uint16_t portNr)
    : SocketConnectionBase  ( )
    , mClientSocket         ( hostName, portNr )
    , mCookie               ( areg::COOKIE_UNKNOWN )
    , mSockSendBuf          ( areg::SOCKET_SEND_BUFFER_SIZE )
    , mSockRecvBuf          ( areg::SOCKET_RECV_BUFFER_SIZE )
    , mSockSendTimeoutMs    ( areg::SOCKET_SEND_TIMEOUT_MS )
{
}

ClientConnection::ClientConnection(const areg::SocketAddress & remoteAddress)
    : SocketConnectionBase  ( )
    , mClientSocket         ( remoteAddress )
    , mCookie               ( areg::COOKIE_UNKNOWN )
    , mSockSendBuf          ( areg::SOCKET_SEND_BUFFER_SIZE )
    , mSockRecvBuf          ( areg::SOCKET_RECV_BUFFER_SIZE )
    , mSockSendTimeoutMs    ( areg::SOCKET_SEND_TIMEOUT_MS )
{
}


bool ClientConnection::create_socket(const String & hostName, uint16_t portNr)
{
    set_cookie( mClientSocket.create(hostName, portNr) ? areg::COOKIE_LOCAL : areg::COOKIE_UNKNOWN );
    if (mClientSocket.is_valid())
    {
        areg::set_send_size(mClientSocket.handle(), mSockSendBuf);
        areg::set_recv_size(mClientSocket.handle(), mSockRecvBuf);
        areg::set_send_timeout(mClientSocket.handle(), mSockSendTimeoutMs);
        areg::socket_set_no_delay(mClientSocket.handle());
    }

    return mClientSocket.is_valid();
}

bool ClientConnection::create_socket()
{
    set_cookie( mClientSocket.create() ? areg::COOKIE_LOCAL : areg::COOKIE_UNKNOWN );
    if (mClientSocket.is_valid())
    {
        areg::set_send_size(mClientSocket.handle(), mSockSendBuf);
        areg::set_recv_size(mClientSocket.handle(), mSockRecvBuf);
        areg::set_send_timeout(mClientSocket.handle(), mSockSendTimeoutMs);
        areg::socket_set_no_delay(mClientSocket.handle());
    }

    return mClientSocket.is_valid();
}

bool ClientConnection::create_socket_fd()
{
    set_cookie( mClientSocket.create_fd() ? areg::COOKIE_LOCAL : areg::COOKIE_UNKNOWN );
    if (mClientSocket.is_valid())
    {
        areg::set_send_size(mClientSocket.handle(), mSockSendBuf);
        areg::set_recv_size(mClientSocket.handle(), mSockRecvBuf);
        areg::set_send_timeout(mClientSocket.handle(), mSockSendTimeoutMs);
    }

    return mClientSocket.is_valid();
}

void ClientConnection::close_socket()
{
    set_cookie(areg::COOKIE_UNKNOWN);
    mClientSocket.close();
}

} // namespace areg
