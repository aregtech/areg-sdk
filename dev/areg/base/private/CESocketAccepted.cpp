/************************************************************************
 * \file        areg/base/private/CESocketAccepted.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform Accepted Socket class implementation.
 ************************************************************************/

#include "areg/base/CESocketAccepted.hpp"

CESocketAccepted::CESocketAccepted( void )
    : CESocket  ( )
{
    ; // do nothing
}

CESocketAccepted::CESocketAccepted( const SOCKETHANDLE hSocket, const NESocket::CEInterlockedValue & sockAddress )
    : CESocket  ( hSocket, sockAddress)
{
    ; // do nothing
}

CESocketAccepted::CESocketAccepted( const CESocketAccepted & source )
    : CESocket  ( static_cast<const CESocket &>(source) )
{
    ; // do nothing
}

CESocketAccepted::~CESocketAccepted(void)
{
    ; // do nothing
}

bool CESocketAccepted::CreateSocket(const char * /*hostName*/, unsigned short /*portNr*/)
{
    return true;
}

bool CESocketAccepted::CreateSocket(void)
{
    return true;
}

const CESocketAccepted & CESocketAccepted::operator = ( const CESocketAccepted & source )
{
    if ( static_cast<CESocket *>(this) != &source )
    {
        DecreaseLock();

        mSocket     = source.mSocket;
        mLockCount  = source.mLockCount;
        mAddress    = source.mAddress;

        IncreaseLock();
    }
    return (*this);
}

bool CESocketAccepted::operator == (const CESocketAccepted & other) const
{
    return ( this != &other && mSocket != NESocket::InvalidSocketHandle ? mSocket == other.mSocket : false );
}

bool CESocketAccepted::SetAddress(const char * /*hostName*/, unsigned short /*portNr*/, bool /*isServer*/)
{
    return false;
}

void CESocketAccepted::SetAddress(const NESocket::CEInterlockedValue & /*newAddress*/)
{
    ;
}
