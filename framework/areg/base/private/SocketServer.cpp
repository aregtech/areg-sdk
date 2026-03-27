/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/SocketServer.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform Server Socket class declaration.
 ************************************************************************/
#include "areg/base/SocketServer.hpp"
#include "areg/base/SocketAccepted.hpp"
namespace areg {

SocketServer::SocketServer( const String& hostName, uint16_t portNr )
    : Socket  ( )
{
    mAddress.resolve_address(hostName.is_empty() ? areg::LocalHost : hostName, portNr, true);
}

SocketServer::SocketServer(const char* hostName, uint16_t portNr)
    : Socket()
{
    mAddress.resolve_address(areg::is_empty(hostName) ? areg::LocalHost : hostName, portNr, true);
}

SocketServer::SocketServer( const areg::SocketAddress & serverAddress )
    : Socket  ( )
{
    mAddress = serverAddress;
}

bool SocketServer::create(const String& hostName, uint16_t portNr)
{
    return (mAddress.resolve_address(hostName, portNr, true) && create());
}

bool SocketServer::create(const char* hostName, uint16_t portNr)
{
    return (mAddress.resolve_address(hostName, portNr, true) && create());
}

bool SocketServer::create()
{
    decrease_lock();
    if ( mAddress.is_valid() )
    {
        const SOCKETHANDLE hSocket = areg::server_connect(mAddress);
        if ( hSocket != areg::InvalidSocketHandle )
        {
            mSocket   = SocketHandle(hSocket);
            mSendSize = areg::max_send_size(hSocket);
            mRecvSize = areg::max_receive_size(hSocket);
        }
    }

    return is_valid();
}

bool SocketServer::listen(int32_t maxQueueSize)
{
    return (is_valid() ? areg::server_listen(mSocket.value(), maxQueueSize > 0 ? maxQueueSize : areg::MAXIMUM_LISTEN_QUEUE_SIZE) : false );
}

SOCKETHANDLE SocketServer::wait_connection_event(areg::SocketMultiplexer & multiplexer, areg::SocketAddress & addrAccepted)
{
    return ( is_valid() ? areg::server_accept(multiplexer, mSocket.value(), &addrAccepted) : areg::InvalidSocketHandle );
}

SOCKETHANDLE SocketServer::wait_connection_event(areg::SocketAddress & addrAccepted, const SOCKETHANDLE * masterList, int32_t entriesCount)
{
    return ( is_valid() ? areg::server_accept(mSocket.value(), masterList, entriesCount, &addrAccepted) : areg::InvalidSocketHandle );
}

} // namespace areg
