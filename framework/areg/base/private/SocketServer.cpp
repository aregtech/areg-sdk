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
 * \brief       Areg Platform Server Socket class implementation.
 ************************************************************************/
#include "areg/base/SocketServer.hpp"
#include "areg/base/SocketAccepted.hpp"

namespace areg
{
    SocketServer::SocketServer( const char * hostName, uint16_t portNr )
        : Socket  ( )
    {
        mAddress.resolveAddress(hostName != nullptr ? hostName : LocalHost, portNr, true);
    }

    SocketServer::SocketServer( const SocketAddress & serverAddress )
        : Socket  ( )
    {
        mAddress = serverAddress;
    }

    bool SocketServer::createSocket(const char * hostName, uint16_t portNr)
    {
        return ( mAddress.resolveAddress(hostName, portNr, true) && createSocket( ) );
    }

    bool SocketServer::createSocket()
    {
        decreaseLock();
        if ( mAddress.isValid() )
        {
            SOCKETHANDLE hSocket = serverSocketConnect(static_cast<const char *>(mAddress.getHostAddress()), mAddress.getHostPort());
            if ( hSocket != InvalidSocketHandle )
            {
                mSocket = std::make_shared<SOCKETHANDLE>( hSocket );
                mSendSize = getMaxSendSize(hSocket);
                mRecvSize = getMaxReceiveSize(hSocket);
            }
        }

        return isValid();
    }

    bool SocketServer::listenConnection(int32_t maxQueueSize)
    {
        return (isValid() ? serverListenConnection(*mSocket, maxQueueSize > 0 ? maxQueueSize : MAXIMUM_LISTEN_QUEUE_SIZE) : false );
    }

    SOCKETHANDLE SocketServer::waitConnectionEvent(SocketAddress & out_addrAccepted, const SOCKETHANDLE * masterList, int32_t entriesCount)
    {
        return ( isValid() ? serverAcceptConnection(*mSocket, masterList, entriesCount, &out_addrAccepted) : InvalidSocketHandle );
    }

} // namespace areg