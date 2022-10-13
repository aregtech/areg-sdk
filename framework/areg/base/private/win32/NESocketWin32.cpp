/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/base/private/win32/NESocketWin32.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform. Socket POSIX specific wrappers methods
 ************************************************************************/
#include "areg/base/NESocket.hpp"

#ifdef  _WINDOWS

#include "areg/base/SynchObjects.hpp"
#include "areg/base/GEMacros.h"
#include "areg/base/NEMemory.hpp"

#ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
#endif  // WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>

#include <atomic>

//////////////////////////////////////////////////////////////////////////
// Local static members
//////////////////////////////////////////////////////////////////////////

namespace
{
	/**
	 * \brief   Global socket initialize / release counter.
	 *          Initialize socket in process if counter is changing from 0 to 1.
	 *          Release socket in frees resources in process when counter reaches 0.
	 **/
	std::atomic_uint _instanceCount( 0u );

} // namespace

//////////////////////////////////////////////////////////////////////////
// NESocket namespace functions implementation
//////////////////////////////////////////////////////////////////////////

AREG_API_IMPL bool NESocket::socketInitialize(void)
{
    bool result = true;
    if ( _instanceCount.fetch_add(1) == 0 )
    {
        WSADATA wsaData;
        ::memset(&wsaData, 0, sizeof(WSADATA));
        if ( ::WSAStartup(MAKEWORD( 2, 2 ), &wsaData) != 0 )
        {
            OUTPUT_ERR("Failed to initialize Windows Socket of version 2.2, error code [ %p ]", static_cast<id_type>(::WSAGetLastError()));

            result = false;
            _instanceCount.fetch_sub(1);
        }
    }

    return result;
}

AREG_API_IMPL void NESocket::socketRelease(void)
{
    if ( _instanceCount.fetch_sub(1) == 1 )
    {
        OUTPUT_INFO("Releasing socket, no more instances are created");
        ::WSACleanup();
    }
}

AREG_API_IMPL void NESocket::socketClose(SOCKETHANDLE hSocket)
{
    if ( hSocket != NESocket::InvalidSocketHandle )
    {
        shutdown(hSocket, SD_BOTH);
        closesocket( hSocket );
    }
}

AREG_API_IMPL int NESocket::sendData(SOCKETHANDLE hSocket, const unsigned char * dataBuffer, int dataLength, int blockMaxSize /*= NECommon::DEFAULT_SIZE*/ )
{
    int result = -1;
    if ( hSocket != NESocket::InvalidSocketHandle )
    {
        result = 0;
        if ( (dataBuffer != nullptr) && (dataLength > 0) )
        {
            blockMaxSize    = blockMaxSize > 0 ? blockMaxSize : NESocket::getMaxSendSize(hSocket);
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
                        blockMaxSize = NESocket::getMaxSendSize(hSocket);
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

AREG_API_IMPL int NESocket::receiveData(SOCKETHANDLE hSocket, unsigned char * dataBuffer, int dataLength, int blockMaxSize /*= NECommon::DEFAULT_SIZE*/ )
{
    int result = -1;

    if ( hSocket != NESocket::InvalidSocketHandle )
    {
        result = 0;
        if ( (dataBuffer != nullptr) && (dataLength > 0) )
        {
            blockMaxSize    = blockMaxSize > 0 ? blockMaxSize : NESocket::getMaxReceiveSize(hSocket);
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
                        blockMaxSize = NESocket::getMaxReceiveSize(hSocket);
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

AREG_API_IMPL bool NESocket::disableSend(SOCKETHANDLE hSocket)
{
    return ( hSocket != NESocket::InvalidSocketHandle ? RETURNED_OK == shutdown(hSocket, SD_SEND    ) : false );
}

AREG_API_IMPL bool NESocket::disableReceive(SOCKETHANDLE hSocket)
{
    return ( hSocket != NESocket::InvalidSocketHandle ? RETURNED_OK == shutdown( hSocket, SD_RECEIVE) : false );
}

AREG_API_IMPL unsigned int NESocket::remainDataRead( SOCKETHANDLE hSocket )
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
