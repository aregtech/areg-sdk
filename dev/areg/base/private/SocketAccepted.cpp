/************************************************************************
 * \file        areg/base/private/SocketAccepted.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform Accepted Socket class implementation.
 ************************************************************************/

#include "areg/base/SocketAccepted.hpp"

SocketAccepted::SocketAccepted( void )
    : Socket  ( )
{
    ; // do nothing
}

SocketAccepted::SocketAccepted( const SOCKETHANDLE hSocket, const NESocket::InterlockedValue & sockAddress )
    : Socket  ( hSocket, sockAddress)
{
    ; // do nothing
}

SocketAccepted::SocketAccepted( const SocketAccepted & source )
    : Socket  ( static_cast<const Socket &>(source) )
{
    ; // do nothing
}

SocketAccepted::~SocketAccepted(void)
{
    ; // do nothing
}

bool SocketAccepted::createSocket(const char * /*hostName*/, unsigned short /*portNr*/)
{
    return true;
}

bool SocketAccepted::createSocket(void)
{
    return true;
}

const SocketAccepted & SocketAccepted::operator = ( const SocketAccepted & source )
{
    if ( static_cast<Socket *>(this) != &source )
    {
        decreaseLock();

        mSocket     = source.mSocket;
        mLockCount  = source.mLockCount;
        mAddress    = source.mAddress;

        increaseLock();
    }
    return (*this);
}

bool SocketAccepted::operator == (const SocketAccepted & other) const
{
    return ( this != &other && mSocket != NESocket::InvalidSocketHandle ? mSocket == other.mSocket : false );
}

bool SocketAccepted::setAddress(const char * /*hostName*/, unsigned short /*portNr*/, bool /*isServer*/)
{
    return false;
}

void SocketAccepted::setAddress(const NESocket::InterlockedValue & /*newAddress*/)
{
    ;
}
