/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/base/private/posix/NESocketPosix.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform. Socket POSIX specific wrappers methods
 ************************************************************************/
#include "areg/base/NESocket.hpp"

#if defined(_POSIX) || defined(POSIX)

#include "areg/base/SynchObjects.hpp"
#include "areg/base/GEMacros.h"
#include "areg/base/NEMemory.hpp"
#include "areg/trace/GETrace.h"

#include <unistd.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <arpa/inet.h>
#include <ctype.h>      // IEEE Std 1003.1-2001
#include <atomic>

//////////////////////////////////////////////////////////////////////////
// Local static members
//////////////////////////////////////////////////////////////////////////

DEF_TRACE_SCOPE(areg_base_NESocketPosix_socketClose);
DEF_TRACE_SCOPE(areg_base_NESocketPosix_sendData);
DEF_TRACE_SCOPE(areg_base_NESocketPosix_receiveData);
DEF_TRACE_SCOPE(areg_base_NESocketPosix_remainDataRead);

//////////////////////////////////////////////////////////////////////////
// NESocket namespace functions implementation
//////////////////////////////////////////////////////////////////////////

AREG_API bool NESocket::socketInitialize(void)
{
    static_assert( std::atomic_uint::is_always_lock_free );
    return true;
}

AREG_API void NESocket::socketRelease(void)
{
}

AREG_API void NESocket::socketClose(SOCKETHANDLE hSocket)
{
    TRACE_SCOPE(areg_base_NESocketPosix_socketClose);

    if ( hSocket != NESocket::InvalidSocketHandle )
    {
        TRACE_WARN("Closing socket [ %p ]", hSocket);

        shutdown(hSocket, SHUT_RDWR);
        close( hSocket );
    }
    else
    {
        TRACE_DBG("Socket is invalid, ignore closing socket");
    }
}

AREG_API int NESocket::sendData(SOCKETHANDLE hSocket, const unsigned char * dataBuffer, int dataLength, int blockMaxSize /*= -1*/ )
{
    TRACE_SCOPE(areg_base_NESocketPosix_sendData);

    int result = -1;

    if ( hSocket != NESocket::InvalidSocketHandle )
    {
        result = 0;
        if ( (dataBuffer != nullptr) && (dataLength > 0) )
        {
            bool checkSize  = false;
            blockMaxSize    = blockMaxSize > 0 ? blockMaxSize : NESocket::getMaxSendSize(hSocket);
            result          = dataLength;

            TRACE_DBG("Going to send data, length = [ %d ], blockMaxSize [ %d ], socket is VALID", dataLength, blockMaxSize);
            while ( dataLength > 0 )
            {
                int remain = dataLength > blockMaxSize ? blockMaxSize : dataLength;
                TRACE_DBG("Sending [ %d ] bytes of data", remain);
                int written= send(hSocket, reinterpret_cast<const char *>(dataBuffer), remain, 0);
                if ( written > 0 )
                {
                    dataLength -= written;
                    dataBuffer += written;
                    TRACE_DBG("Has sent [ %d ] bytes of data, there is still [ %d ] bytes left", written, dataLength);
                }
                else
                {
                    if ( (checkSize == false) && (errno == EMSGSIZE) )
                    {
                        // try again with other package size
                        checkSize   = true;
                        blockMaxSize= NESocket::getMaxSendSize(hSocket);
                        TRACE_WARN("No data has sent, trying to change the maximum block size to [ %d ] bytes and try again, there are still [ %d ] bytes to sent", blockMaxSize, dataLength);
                    }
                    else
                    {
                        TRACE_ERR("FAILED to sent [ %d ] bytes of data, error code is [ %p ], terminating sending", dataLength, static_cast<id_type>(errno));
                        // in all other cases
                        dataLength  = 0;    // break loop
                        result      = -1;   // notify failure
                    }
                }
            }
        }
        else
        {
            TRACE_ERR("Either buffer is nullptr [ %s ] or wrong data length to sent [ %d ]", dataBuffer == nullptr ? "YES" : "NO", dataLength);
        }
    }
    else
    {
        TRACE_ERR("INVALID socket, will not be able to sent [ %d ] bytes of data", dataLength);
    }

    return result;
}

AREG_API int NESocket::receiveData(SOCKETHANDLE hSocket, unsigned char * dataBuffer, int dataLength, int blockMaxSize /*= -1*/ )
{
    TRACE_SCOPE(areg_base_NESocketPosix_receiveData);

    int result = -1;
    if ( hSocket != NESocket::InvalidSocketHandle )
    {
        result = 0;
        if ( (dataBuffer != nullptr) && (dataLength > 0) )
        {
            blockMaxSize    = blockMaxSize > 0 ? blockMaxSize : NESocket::getMaxReceiveSize(hSocket);
            TRACE_DBG("Going to receive data, available space [ %d ] bytes, blockMaxSize [ %d ] bytes", dataLength, blockMaxSize);

            while ( dataLength > 0 )
            {
                int remain = dataLength > blockMaxSize ? blockMaxSize : dataLength;
                TRACE_DBG("Receiving [ %d ] bytes of data", remain);
                int read   = recv(hSocket, dataBuffer + result, remain, 0);
                if ( read > 0 )
                {
                    dataLength -= read;
                    result     += read;
                    TRACE_DBG("Has read [ %d ] bytes of data, there is still [ %d ] bytes of data to wait, totally received [ %d ] bytes", read, dataLength, result);
                }
                else if ( read == 0 )
                {
                    TRACE_INFO("No data to read. There are still [ %d ] bytes to wait, breaking loop", dataLength);
                    dataLength  = 0;    // break loop. the other side disconnected
                    result      = 0;    // no data could read. specified socket is closed
                }
                else
                {
                    TRACE_ERR("FAILED to receive [ %d ] bytes of data, error code is [ %p ], terminating receiving", dataLength, static_cast<id_type>(errno));
                    // in all other cases
                    dataLength  = 0;    // break loop
                    result      = -1;   // notify failure
                }
            }
        }
        else
        {
            TRACE_ERR("Either buffer is nullptr [ %s ] or wrong data length to receive [ %d ]", dataBuffer == nullptr ? "YES" : "NO", dataLength);
        }
    }
    else
    {
        TRACE_ERR("INVALID socket, will not be able to receive [ %d ] bytes of data", dataLength);
    }

    return result;
}

AREG_API bool NESocket::disableSend(SOCKETHANDLE hSocket)
{
    return ( hSocket != NESocket::InvalidSocketHandle ? RETURNED_OK == shutdown( hSocket, SHUT_WR) : false );
}

AREG_API bool NESocket::disableReceive(SOCKETHANDLE hSocket)
{
    return ( hSocket != NESocket::InvalidSocketHandle ? RETURNED_OK == shutdown(hSocket, SHUT_RD ) : false );
}

AREG_API unsigned int NESocket::remainDataRead( SOCKETHANDLE hSocket )
{
    TRACE_SCOPE(areg_base_NESocketPosix_remainDataRead);

    unsigned int result = 0;
    if ( hSocket != NESocket::InvalidSocketHandle )
    {
        u_long arg = 0L;
        if ( RETURNED_OK  == ioctl( hSocket, FIONREAD, &arg) )
        {
            result = static_cast<unsigned int>(arg);
        }
        else
        {
            TRACE_WARN("FAILED to check the remaining data to read, error code [ %p ]", static_cast<id_type>(errno));
        }
    }

    TRACE_INFO("Remain [ %d ] bytes of data to read", result);

    return result;
}

#endif  // defined(_POSIX) || defined(POSIX)
