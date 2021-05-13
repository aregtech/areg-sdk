/************************************************************************
 * \file        areg/base/private/Socket.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform base class of socket.
 ************************************************************************/
#include "areg/base/Socket.hpp"

#include "areg/base/NEUtilities.hpp"
#include "areg/base/ESynchObjects.hpp"
#include "areg/base/RemoteMessage.hpp"


//////////////////////////////////////////////////////////////////////////
// Socket class implementation
//////////////////////////////////////////////////////////////////////////

Socket::Socket( void )
    : mSocket   ( NESocket::InvalidSocketHandle )
    , mLockCount( NULL )
    , mAddress  ( )
{
    static_cast<void>(NESocket::socketInitialize( ));
}

Socket::Socket(const SOCKETHANDLE hSocket, const NESocket::InterlockedValue & sockAddress)
    : mSocket   ( hSocket )
    , mLockCount( hSocket != NESocket::InvalidSocketHandle ? DEBUG_NEW unsigned int : NULL )
    , mAddress  ( sockAddress )
{
    if ( mLockCount != NULL )
        *mLockCount = 1;

    static_cast<void>(NESocket::socketInitialize( ));
}

Socket::Socket( const Socket & source )
    : mSocket   ( source.mSocket )
    , mLockCount( source.mLockCount )
    , mAddress  ( source.mAddress )
{
    static_cast<void>(NESocket::socketInitialize( ));
    increaseLock();
}

Socket::~Socket(void)
{
    decreaseLock();
    static_cast<void>(NESocket::socketRelease());
}

void Socket::closeSocket(void)
{
    decreaseLock( );
}

int Socket::sendData( const unsigned char * buffer, int length ) const
{
    return NESocket::sendData( mSocket, buffer, length, -1 );
}

int Socket::receiveData( unsigned char * buffer, int length ) const
{
    return NESocket::receiveData( mSocket, buffer, length, -1 );
}

bool Socket::setAddress(const char * hostName, unsigned short portNr, bool isServer)
{
    if ( isValid() && (mAddress.getHostAddress() != hostName || mAddress.getHostPort() != portNr) )
        decreaseLock( );
    return mAddress.resolveAddress(hostName, portNr, isServer);
}

void Socket::increaseLock(void)
{
    if ( isValid() )
    {
        ASSERT( mLockCount != NULL );
        InterlockedValue lock( mLockCount );
        lock.increment();
    }
}

void Socket::decreaseLock(void)
{
    if ( isValid() )
    {
        ASSERT( mLockCount != NULL );
        SOCKETHANDLE hSocket = mSocket;
        unsigned int *counter= mLockCount;
        mSocket     = NESocket::InvalidSocketHandle;
        mLockCount  = NULL;

        InterlockedValue lock( counter );
        if ( lock.testDecrementZero() )
        {
            delete counter;
            closeSocketHandle(hSocket);
        }
    }
}

void Socket::closeSocketHandle( SOCKETHANDLE hSocket )
{
    if ( hSocket != NESocket::InvalidSocketHandle )
    {
        NESocket::socketClose(hSocket);
    }
}

unsigned int Socket::remainRead( void ) const
{
    return NESocket::remainDataRead(mSocket);
}
