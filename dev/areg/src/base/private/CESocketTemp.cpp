/************************************************************************
 * \file        areg/src/base/private/CESocketTemp.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform temporary socket class.
 ************************************************************************/
#include "areg/src/base/CESocketTemp.hpp"

CESocketTemp::CESocketTemp( SOCKETHANDLE hSocket )
    : CESocket  ( hSocket, NESocket::CEInterlockedValue() )
{
    ; // do nothing
}

CESocketTemp::CESocketTemp( SOCKETHANDLE hSocket, const NESocket::CEInterlockedValue & address )
    : CESocket  ( hSocket, address )
{
    ; // do nothing
}

CESocketTemp::CESocketTemp( const CESocketTemp & src )
    : CESocket  ( static_cast<const CESocket &>(src) )
{
    ; // do nothing
}

CESocketTemp::~CESocketTemp( void )
{
    ; // do nothing
}

bool CESocketTemp::CreateSocket( const char * /*hostName*/, unsigned short /*portNr*/ )
{
    return NESocket::IsSocketHandleValid(mSocket);
}

bool CESocketTemp::CreateSocket( void )
{
    return NESocket::IsSocketHandleValid( mSocket );
}

void CESocketTemp::CloseSocketHandle( SOCKETHANDLE /*hSocket*/ )
{
    ; // do nothing
}
