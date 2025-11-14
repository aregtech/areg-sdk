/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/base/private/SocketServer.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform Server Socket class declaration.
 ************************************************************************/
#include "areg/base/SocketServer.hpp"
#include "areg/base/SocketAccepted.hpp"

SocketServer::SocketServer( const char * hostName, unsigned short portNr )
    : Socket  ( )
{
    mAddress.resolveAddress(hostName != nullptr ? hostName : NESocket::LocalHost, portNr, true);
}

SocketServer::SocketServer( const NESocket::SocketAddress & serverAddress )
    : Socket  ( )
{
    mAddress = serverAddress;
}

bool SocketServer::createSocket(const char * hostName, unsigned short portNr)
{
    return ( mAddress.resolveAddress(hostName, portNr, true) && createSocket( ) );
}

bool SocketServer::createSocket(void)
{
    decreaseLock();
    if ( mAddress.isValid() )
    {
    	SOCKETHANDLE hSocket = NESocket::serverSocketConnect(static_cast<const char *>(mAddress.getHostAddress()), mAddress.getHostPort());
        if ( hSocket != NESocket::InvalidSocketHandle )
        {
            mSocket = std::make_shared<SOCKETHANDLE>( hSocket );
            mSendSize = NESocket::getMaxSendSize(hSocket);
            mRecvSize = NESocket::getMaxReceiveSize(hSocket);
        }
    }

    return isValid();
}

bool SocketServer::listenConnection(int maxQueueSize)
{
    return (isValid() ? NESocket::serverListenConnection(*mSocket, maxQueueSize > 0 ? maxQueueSize : NESocket::MAXIMUM_LISTEN_QUEUE_SIZE) : false );
}

SOCKETHANDLE SocketServer::waitConnectionEvent(NESocket::SocketAddress & out_addrAccepted, const SOCKETHANDLE * masterList, int entriesCount)
{
    return ( isValid() ? NESocket::serverAcceptConnection(*mSocket, masterList, entriesCount, &out_addrAccepted) : NESocket::InvalidSocketHandle );
}
