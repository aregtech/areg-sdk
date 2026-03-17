/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/SocketClient.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform base class of socket.
 ************************************************************************/
#include "areg/base/SocketClient.hpp"

namespace areg {

SocketClient::SocketClient( const char* hostName, uint16_t portNr )
    : Socket  ( )
{
    mAddress.resolve_address(areg::is_empty(hostName) ? areg::LocalHost : hostName, portNr, false);
}

SocketClient::SocketClient(const String& hostName, uint16_t portNr)
    : Socket()
{
    mAddress.resolve_address(hostName.is_empty() ? areg::LocalHost : hostName, portNr, false);
}

SocketClient::SocketClient(const areg::SocketAddress & remoteAddress)
    : Socket  ( )
{
    mAddress = remoteAddress;
}

bool SocketClient::create(const String& hostName, uint16_t portNr)
{
    return (mAddress.resolve_address(hostName, portNr, false) && create());
}

bool SocketClient::create(const char * hostName, uint16_t portNr)
{
    return ( mAddress.resolve_address(hostName, portNr, false) && create());
}

bool SocketClient::create()
{
    decrease_lock();

    if ( mAddress.is_valid() )
    {
        SOCKETHANDLE hSocket = areg::client_connect(mAddress);
        if ( hSocket != areg::InvalidSocketHandle )
        {
            mSocket   = std::make_shared<SOCKETHANDLE>(hSocket);
            mSendSize = areg::max_send_size(hSocket);
            mRecvSize = areg::max_receive_size(hSocket);
        }
    }

    return is_valid();
}

} // namespace areg
