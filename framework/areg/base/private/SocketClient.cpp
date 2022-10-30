/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2022 Aregtech UG. All rights reserved.
 * \file        areg/base/private/SocketClient.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform base class of socket.
 ************************************************************************/
#include "areg/base/SocketClient.hpp"

SocketClient::SocketClient( const char * hostName, unsigned short portNr )
    : Socket  ( )
{
    mAddress.resolveAddress(hostName != nullptr ? hostName : NESocket::LocalHost, portNr, false);
}

SocketClient::SocketClient(const NESocket::SocketAddress & remoteAddress)
    : Socket  ( )
{
    mAddress = remoteAddress;
}

bool SocketClient::createSocket(const char * hostName, unsigned short portNr)
{
    return ( mAddress.resolveAddress(hostName, portNr, false) && createSocket());
}

bool SocketClient::createSocket( void )
{
    decreaseLock();

    if ( mAddress.isValid() )
    {
    	SOCKETHANDLE hSocket = NESocket::clientSocketConnect(static_cast<const char *>(mAddress.getHostAddress()), mAddress.getHostPort());
        if ( hSocket != NESocket::InvalidSocketHandle )
        {
        	mSocket = std::make_shared<SOCKETHANDLE>(hSocket);
        }
    }

    return isValid();
}
