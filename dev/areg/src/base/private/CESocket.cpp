/************************************************************************
 * \file        areg/src/base/private/CESocket.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform base class of socket.
 ************************************************************************/
#include "areg/src/base/CESocket.hpp"

#include "areg/src/base/NEUtilities.hpp"
#include "areg/src/base/ESynchObjects.hpp"
#include "areg/src/base/CERemoteMessage.hpp"


//////////////////////////////////////////////////////////////////////////
// CESocket class implementation
//////////////////////////////////////////////////////////////////////////

CESocket::CESocket( void )
    : mSocket   ( NESocket::InvalidSocketHandle )
    , mLockCount( NULL )
    , mAddress  ( )
{
    static_cast<void>(NESocket::SocketInitialize( ));
}

CESocket::CESocket(const SOCKETHANDLE hSocket, const NESocket::CEInterlockedValue & sockAddress)
    : mSocket   ( hSocket )
    , mLockCount( hSocket != NESocket::InvalidSocketHandle ? DEBUG_NEW unsigned int : NULL )
    , mAddress  ( sockAddress )
{
    if ( mLockCount != NULL )
        *mLockCount = 1;
    static_cast<void>(NESocket::SocketInitialize( ));
}

CESocket::CESocket( const CESocket & source )
    : mSocket   ( source.mSocket )
    , mLockCount( source.mLockCount )
    , mAddress  ( source.mAddress )
{
    static_cast<void>(NESocket::SocketInitialize( ));
    IncreaseLock();
}

CESocket::~CESocket(void)
{
    DecreaseLock();
    static_cast<void>(NESocket::SocketRelease());
}

void CESocket::CloseSocket(void)
{
    DecreaseLock( );
}

int CESocket::SendData( const unsigned char * buffer, int length ) const
{
    return NESocket::SendData( mSocket, buffer, length, -1 );
}

int CESocket::ReceiveData( unsigned char * buffer, int length ) const
{
    return NESocket::ReceiveData( mSocket, buffer, length, -1 );
}

bool CESocket::SetAddress(const char * hostName, unsigned short portNr, bool isServer)
{
    if ( IsValid() && (mAddress.GetHostAddress() != hostName || mAddress.GetHostPort() != portNr) )
        DecreaseLock( );
    return mAddress.ResolveAddress(hostName, portNr, isServer);
}

void CESocket::IncreaseLock(void)
{
    if ( IsValid() )
    {
        ASSERT( mLockCount != NULL );
        CEInterlockedValue lock( mLockCount );
        lock.Increment();
    }
}

void CESocket::DecreaseLock(void)
{
    if ( IsValid() )
    {
        ASSERT( mLockCount != NULL );
        SOCKETHANDLE hSocket = mSocket;
        unsigned int *counter= mLockCount;
        mSocket     = NESocket::InvalidSocketHandle;
        mLockCount  = NULL;

        CEInterlockedValue lock( counter );
        if ( lock.TestDecrementZero() )
        {
            delete counter;
            CloseSocketHandle(hSocket);
        }
    }
}

void CESocket::CloseSocketHandle( SOCKETHANDLE hSocket )
{
    if ( hSocket != NESocket::InvalidSocketHandle )
    {
        NESocket::SocketClose(hSocket);
    }
}

unsigned int CESocket::RemainRead( void ) const
{
    return NESocket::GetRemainingDataRead(mSocket);
}
