/************************************************************************
 * \file        areg/base/private/SocketTemp.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform temporary socket class.
 ************************************************************************/
#include "areg/base/SocketTemp.hpp"

SocketTemp::SocketTemp( SOCKETHANDLE hSocket )
    : Socket  ( hSocket, NESocket::InterlockedValue() )
{
    ; // do nothing
}

SocketTemp::SocketTemp( SOCKETHANDLE hSocket, const NESocket::InterlockedValue & address )
    : Socket  ( hSocket, address )
{
    ; // do nothing
}

SocketTemp::SocketTemp( const SocketTemp & src )
    : Socket  ( static_cast<const Socket &>(src) )
{
    ; // do nothing
}

SocketTemp::~SocketTemp( void )
{
    ; // do nothing
}

bool SocketTemp::createSocket( const char * /*hostName*/, unsigned short /*portNr*/ )
{
    return NESocket::isSocketHandleValid(mSocket);
}

bool SocketTemp::createSocket( void )
{
    return NESocket::isSocketHandleValid( mSocket );
}

void SocketTemp::closeSocketHandle( SOCKETHANDLE /*hSocket*/ )
{
    ; // do nothing
}
