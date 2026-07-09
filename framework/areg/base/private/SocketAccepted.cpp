/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        areg/base/private/SocketAccepted.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform Accepted Socket class implementation.
 ************************************************************************/

#include "areg/base/SocketAccepted.hpp"

namespace areg {

SocketAccepted::SocketAccepted( const SOCKETHANDLE hSocket, const areg::SocketAddress & sockAddress )
    : Socket  ( hSocket, sockAddress)
{
}

SocketAccepted::SocketAccepted(const SOCKETHANDLE hSocket, areg::SocketAddress&& sockAddress) noexcept
    : Socket(hSocket, std::move(sockAddress))
{
}

bool SocketAccepted::create(const String& /*hostName*/, uint16_t /*portNr*/)
{
    return true;
}

bool SocketAccepted::create(const char* /*hostName*/, uint16_t /*portNr*/)
{
    return true;
}

bool SocketAccepted::create()
{
    return true;
}

bool SocketAccepted::operator == (const SocketAccepted & other) const noexcept
{
    return (this == &other) || ( is_valid() && (mSocket.value() == other.handle()) );
}

bool SocketAccepted::set_address(const char * /*hostName*/, uint16_t /*portNr*/, bool /*isServer*/)
{
    return false;
}

void SocketAccepted::set_address(const areg::SocketAddress & /*newAddress*/)
{
}

} // namespace areg
