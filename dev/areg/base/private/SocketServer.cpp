/************************************************************************
 * \file        areg/base/private/SocketServer.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform Server Socket class declaration.
 ************************************************************************/
#include "areg/base/SocketServer.hpp"
#include "areg/base/SocketAccepted.hpp"

SocketServer::SocketServer( void )
    : Socket  ( )
{
    ; // do nothing
}

SocketServer::SocketServer( const char * hostName, unsigned short portNr )
    : Socket  ( )
{
    mAddress.resolveAddress(hostName != NULL ? hostName : NESocket::LocalHost, portNr, true);
}

SocketServer::SocketServer( const NESocket::InterlockedValue & serverAddress )
    : Socket  ( )
{
    mAddress = serverAddress;
}

SocketServer::~SocketServer(void)
{
    ;
}

bool SocketServer::createSocket(const char * hostName, unsigned short portNr)
{
    mAddress.resolveAddress(hostName, portNr, true);
    return createSocket();
}

bool SocketServer::createSocket(void)
{
    decreaseLock();
    if ( mAddress.isValid() )
    {
        mSocket = NESocket::serverSocketConnect(static_cast<const char *>(mAddress.getHostAddress()), mAddress.getHostPort());
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

bool SocketServer::listenConnection(int maxQueueSize)
{
    return (isValid() ? NESocket::serverListenConnection(mSocket, maxQueueSize > 0 ? maxQueueSize : NESocket::MAXIMUM_LISTEN_QUEUE_SIZE) : false );
}

SOCKETHANDLE SocketServer::waitConnectionEvent(NESocket::InterlockedValue & out_addrAccepted, const SOCKETHANDLE * masterList, int entriesCount)
{
    return ( isValid() ? NESocket::serverAcceptConnection(mSocket, masterList, entriesCount, &out_addrAccepted) : NESocket::InvalidSocketHandle );
}
