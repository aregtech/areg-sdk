/************************************************************************
 * \file        areg/base/private/SocketClient.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform base class of socket.
 ************************************************************************/
#include "areg/base/SocketClient.hpp"

SocketClient::SocketClient( void )
    : Socket  ( )
{
    ; // do nothing
}

SocketClient::SocketClient( const char * hostName, unsigned int portNr )
    : Socket  ( )
{
    mAddress.resolveAddress(hostName != NULL ? hostName : NESocket::LocalHost, portNr, false);
}

SocketClient::SocketClient(const NESocket::InterlockedValue & remoteAddress)
    : Socket  ( )
{
    mAddress = remoteAddress;
}

SocketClient::~SocketClient(void)
{
    ;
}

bool SocketClient::createSocket(const char * hostName, unsigned short portNr)
{
    mAddress.resolveAddress(hostName, portNr, false);
    return createSocket();
}

bool SocketClient::createSocket( void )
{
    decreaseLock();
    if ( mAddress.isValid() )
    {
        mSocket = NESocket::clientSocketConnect(static_cast<const char *>(mAddress.getHostAddress()), mAddress.getHostPort());
        if ( mSocket != NESocket::InvalidSocketHandle )
        {
            ASSERT( mLockCount == NULL );
            mLockCount = DEBUG_NEW unsigned int;
            if ( mLockCount != NULL )
                *mLockCount = 1;
            else
                closeSocket();
        }
    }
    return isValid();
}
