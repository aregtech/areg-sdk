/************************************************************************
 * \file        areg/src/base/private/CESocketClient.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform base class of socket.
 ************************************************************************/
#include "areg/src/base/CESocketClient.hpp"

CESocketClient::CESocketClient( void )
    : CESocket  ( )
{
    ; // do nothing
}

CESocketClient::CESocketClient( const char * hostName, unsigned int portNr )
    : CESocket  ( )
{
    mAddress.ResolveAddress(hostName != NULL ? hostName : NESocket::LocalHost, portNr, false);
}

CESocketClient::CESocketClient(const NESocket::CEInterlockedValue & remoteAddress)
    : CESocket  ( )
{
    mAddress = remoteAddress;
}

CESocketClient::~CESocketClient(void)
{
    ;
}

bool CESocketClient::CreateSocket(const char * hostName, unsigned short portNr)
{
    mAddress.ResolveAddress(hostName, portNr, false);
    return CreateSocket();
}

bool CESocketClient::CreateSocket( void )
{
    DecreaseLock();
    if ( mAddress.IsValid() )
    {
        mSocket = NESocket::ClientSocketConnect(static_cast<const char *>(mAddress.GetHostAddress()), mAddress.GetHostPort());
        if ( mSocket != NESocket::InvalidSocketHandle )
        {
            ASSERT( mLockCount == NULL );
            mLockCount = DEBUG_NEW unsigned int;
            if ( mLockCount != NULL )
                *mLockCount = 1;
            else
                CloseSocket();
        }
    }
    return IsValid();
}
