/************************************************************************
 * \file        areg/base/private/win32/NESocketWin32.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform. Socket POSIX specific wrappers methods
 ************************************************************************/
#include "areg/base/NESocket.hpp"

#ifdef  _WINDOWS

#include "areg/base/ESynchObjects.hpp"
#include "areg/base/GEMacros.h"
#include "areg/base/NEMemory.hpp"

#ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
#endif  // WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>

//////////////////////////////////////////////////////////////////////////
// Local static members
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Global socket initialize / release counter.
 *          Initialize socket in process if counter is changing from 0 to 1.
 *          Release socket in frees resources in process when counter reaches 0.
 **/
static CEInterlockedValue   _instanceCount( static_cast<unsigned int>(0));

//////////////////////////////////////////////////////////////////////////
// NESocket namespace functions implementation
//////////////////////////////////////////////////////////////////////////

AREG_API bool NESocket::SocketInitialize(void)
{
    bool result = true;
    if ( _instanceCount.Increment() == 1 )
    {
        WSADATA wsaData;
        ::memset(&wsaData, 0, sizeof(WSADATA));
        if ( ::WSAStartup(MAKEWORD( 2, 2 ), &wsaData) != 0 )
        {
            OUTPUT_ERR("Failed to initialize Windows Socket of version 2.2, error code [ %p ]", :: WSAGetLastError());

            result = false;
            _instanceCount.Decrement();
        }
    }

    return result;
}

AREG_API void NESocket::SocketRelease(void)
{
    if ( _instanceCount.TestDecrementZero() )
    {
        OUTPUT_INFO("Releasing socket, no more instances are created");
        ::WSACleanup();
    }
}

AREG_API void NESocket::SocketClose(SOCKETHANDLE hSocket)
{
    if ( hSocket != NESocket::InvalidSocketHandle )
    {
        shutdown(hSocket, SD_BOTH);
        closesocket( hSocket );
    }
}

AREG_API SOCKETHANDLE NESocket::ServerAcceptConnection(SOCKETHANDLE serverSocket, const SOCKETHANDLE * masterList, int entriesCount, NESocket::CEInterlockedValue * out_socketAddr /*= NULL*/)
{
    OUTPUT_DBG("Checking server socket event, server socket handle [ %u ]", static_cast<unsigned int>(serverSocket));

    if (out_socketAddr != NULL )
        out_socketAddr->ResetAddress();

    SOCKETHANDLE result = NESocket::InvalidSocketHandle;
    if ( serverSocket != NESocket::InvalidSocketHandle )
    {
        struct fd_set readList;
        FD_ZERO(&readList);
        FD_SET( serverSocket, &readList );


        if ( masterList != NULL && entriesCount > 0)
        {
            entriesCount= MACRO_MIN(entriesCount, (FD_SETSIZE - 1));
            for ( int count = 0; count < entriesCount; ++ count)
                readList.fd_array[count + 1] = masterList[count];
            readList.fd_count = entriesCount + 1;
        }

        int selected    = select( static_cast<int>(serverSocket) + 1 /* param is ignored in Win32*/, &readList, NULL, NULL, NULL);
        if ( selected > 0 )
        {
            if ( FD_ISSET(serverSocket, &readList) != 0 )
            {
                // have got new client connection. resolve and get socket
                struct sockaddr_in acceptAddr; // connecting client address information
                int addrLength = sizeof(sockaddr_in);
                NEMemory::ZeroBuffer(&acceptAddr, sizeof(sockaddr_in));

                result = accept( serverSocket, reinterpret_cast<sockaddr *>(&acceptAddr), &addrLength );
                OUTPUT_DBG("Server accepted new connection of client socket [ %u ]", static_cast<unsigned int>(result));
                if ( result != NESocket::InvalidSocketHandle && out_socketAddr != NULL )
                    out_socketAddr->SetHostAddress(acceptAddr);
            }
            else
            {
                OUTPUT_DBG("Have got select event of existing connection, going to resolve socket");

                //  check whether have got connection from existing clients. if 'yes', server can read data.
                for ( int count = 0; result == NESocket::InvalidSocketHandle && count < entriesCount; ++ count )
                {
                    if (FD_ISSET( masterList[count], &readList ) != 0)
                    {
                        result = masterList[count];
                        OUTPUT_DBG("Server selected event of existing client socket [ %u ] connection", static_cast<unsigned int>(result));
                        break;
                    }
                }
            }
        }
        else
        {
            OUTPUT_ERR("Failed to select connection. The server socket [ %u ] might be closed and not valid anymore, return value [ %d ]", static_cast<unsigned int>(serverSocket), selected);
        }
    }
    else
    {
        OUTPUT_ERR("Invalid server socket, ignoring accept connections!");
    }
    return result;
}

AREG_API int NESocket::SendData(SOCKETHANDLE hSocket, const unsigned char * dataBuffer, int dataLength, int blockMaxSize /*= -1*/ )
{
    int result = -1;
    if ( hSocket != NESocket::InvalidSocketHandle )
    {
        result = 0;
        if ( dataBuffer != NULL && dataLength > 0 )
        {
            blockMaxSize    = blockMaxSize > 0 ? blockMaxSize : NESocket::GetMaximumSendSize(hSocket);
            result          = dataLength;
            while ( dataLength > 0 )
            {
                int remain = dataLength > blockMaxSize ? blockMaxSize : dataLength;
                int written= send(hSocket, reinterpret_cast<const char *>(dataBuffer), remain, 0);
                if ( written > 0 )
                {
                    dataLength -= written;
                    dataBuffer += written;
                }
                else
                {
                    int errCode =  ::WSAGetLastError();
                    if ( errCode == static_cast<int>(WSAEMSGSIZE))
                    {
                        // try again with other package size
                        blockMaxSize = NESocket::GetMaximumSendSize(hSocket);
                    }
                    else
                    {
                        // in all other cases
                        dataLength  = 0;    // break loop
                        result      = -1;   // notify failure
                    }
                }
            }
        }
        else
        {
            ; // no data to sent
        }
    }
    else
    {
        ; // invalid socket
    }

    return result;
}

AREG_API int NESocket::ReceiveData(SOCKETHANDLE hSocket, unsigned char * dataBuffer, int dataLength, int blockMaxSize /*= -1*/ )
{
    int result = -1;

    if ( hSocket != NESocket::InvalidSocketHandle )
    {
        result = 0;
        if ( dataBuffer != NULL && dataLength > 0 )
        {
            blockMaxSize    = blockMaxSize > 0 ? blockMaxSize : NESocket::GetMaximumReceiveSize(hSocket);
            while ( dataLength > 0 )
            {
                int remain = dataLength > blockMaxSize ? blockMaxSize : dataLength;
                int read   = recv(hSocket, reinterpret_cast<char *>(dataBuffer) + result, remain, 0);
                if ( read > 0 )
                {
                    dataLength -= read;
                    result     += read;
                }
                else if ( read == 0 )
                {
                    dataLength  = 0;    // break loop. the other side disconnected
                    result      = 0;    // no data could read. specified socket is closed
                }
                else
                {
                    int errCode =  ::WSAGetLastError();
                    if ( errCode == static_cast<int>(WSAEMSGSIZE))
                    {
                        // try again with other package size
                        blockMaxSize = NESocket::GetMaximumReceiveSize(hSocket);
                    }
                    else
                    {
                        // in all other cases
                        dataLength  = 0;    // break loop
                        result      = -1;   // notify failure
                    }
                }
            }
        }
        else
        {
            ; // no data to receive
        }
    }
    else
    {
        ; // invalid socket
    }

    return result;
}

AREG_API bool NESocket::DisableSend(SOCKETHANDLE hSocket)
{
    return ( hSocket != NESocket::InvalidSocketHandle ? RETURNED_OK == shutdown(hSocket, SD_SEND    ) : false );
}

AREG_API bool NESocket::DisableReceive(SOCKETHANDLE hSocket)
{
    return ( hSocket != NESocket::InvalidSocketHandle ? RETURNED_OK == shutdown( hSocket, SD_RECEIVE) : false );
}

AREG_API unsigned int NESocket::GetRemainingDataRead( SOCKETHANDLE hSocket )
{
    unsigned int result = 0;

    if ( hSocket != NESocket::InvalidSocketHandle )
    {
        u_long arg = 0L;
        if ( RETURNED_OK == ioctlsocket( hSocket, FIONREAD, &arg) )
        {
            result = static_cast<unsigned int>(arg);
        }
    }

    return result;
}

#endif  // _WINDOWS
