/************************************************************************
 * \file        areg/base/private/CESocketServer.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform Server Socket class declaration.
 ************************************************************************/
#include "areg/base/CESocketServer.hpp"
#include "areg/base/CESocketAccepted.hpp"

CESocketServer::CESocketServer( void )
    : CESocket  ( )
{
    ; // do nothing
}

CESocketServer::CESocketServer( const char * hostName, unsigned short portNr )
    : CESocket  ( )
{
    mAddress.ResolveAddress(hostName != NULL ? hostName : NESocket::LocalHost, portNr, true);
}

CESocketServer::CESocketServer( const NESocket::CEInterlockedValue & serverAddress )
    : CESocket  ( )
{
    mAddress = serverAddress;
}

CESocketServer::~CESocketServer(void)
{
    ;
}

bool CESocketServer::CreateSocket(const char * hostName, unsigned short portNr)
{
    mAddress.ResolveAddress(hostName, portNr, true);
    return CreateSocket();
}

bool CESocketServer::CreateSocket(void)
{
    DecreaseLock();
    if ( mAddress.IsValid() )
    {
        mSocket = NESocket::ServerSocketConnect(static_cast<const char *>(mAddress.GetHostAddress()), mAddress.GetHostPort());
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

bool CESocketServer::ListenConnection(int maxQueueSize)
{
    return (IsValid() ? NESocket::ServerListenConnection(mSocket, maxQueueSize > 0 ? maxQueueSize : NESocket::MAXIMUM_LISTEN_QUEUE_SIZE) : false );
}

SOCKETHANDLE CESocketServer::WaitForConnectionEvent(NESocket::CEInterlockedValue & out_addrAccepted, const SOCKETHANDLE * masterList, int entriesCount)
{
    return ( IsValid() ? NESocket::ServerAcceptConnection(mSocket, masterList, entriesCount, &out_addrAccepted) : NESocket::InvalidSocketHandle );
}
