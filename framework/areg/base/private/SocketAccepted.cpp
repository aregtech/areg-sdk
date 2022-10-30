/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2022 Aregtech UG. All rights reserved.
 * \file        areg/base/private/SocketAccepted.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform Accepted Socket class implementation.
 ************************************************************************/

#include "areg/base/SocketAccepted.hpp"

SocketAccepted::SocketAccepted( const SOCKETHANDLE hSocket, const NESocket::SocketAddress & sockAddress )
    : Socket  ( hSocket, sockAddress)
{
}

bool SocketAccepted::createSocket(const char * /*hostName*/, unsigned short /*portNr*/)
{
    return true;
}

bool SocketAccepted::createSocket(void)
{
    return true;
}

bool SocketAccepted::operator == (const SocketAccepted & other) const
{
	return (this == &other) || ( isValid() && (*mSocket == other.getHandle() ));
}

bool SocketAccepted::setAddress(const char * /*hostName*/, unsigned short /*portNr*/, bool /*isServer*/)
{
    return false;
}

void SocketAccepted::setAddress(const NESocket::SocketAddress & /*newAddress*/)
{
}
