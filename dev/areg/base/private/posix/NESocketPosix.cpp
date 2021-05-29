/************************************************************************
 * \file        areg/base/private/posix/NESocketPosix.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform. Socket POSIX specific wrappers methods
 ************************************************************************/
#include "areg/base/NESocket.hpp"

#ifdef  _POSIX

#include "areg/base/ESynchObjects.hpp"
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

//////////////////////////////////////////////////////////////////////////
// Local static members
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Global socket initialize / release counter.
 *          Initialize socket in process if counter is changing from 0 to 1.
 *          Release socket in frees resources in process when counter reaches 0.
 **/
static InterlockedValue   _instanceCount( static_cast<unsigned int>(0));

DEF_TRACE_SCOPE(areg_base_NESocketPosix_sendData);
DEF_TRACE_SCOPE(areg_base_NESocketPosix_receiveData);
DEF_TRACE_SCOPE(areg_base_NESocketPosix_remainDataRead);

//////////////////////////////////////////////////////////////////////////
// NESocket namespace functions implementation
//////////////////////////////////////////////////////////////////////////

AREG_API bool NESocket::socketInitialize(void)
{
    return true;
}

AREG_API void NESocket::socketRelease(void)
{
}

AREG_API void NESocket::socketClose(SOCKETHANDLE hSocket)
{
    if ( hSocket != NESocket::InvalidSocketHandle )
    {
        shutdown(hSocket, SHUT_RDWR);
        close( hSocket );
    }
}

AREG_API int NESocket::sendData(SOCKETHANDLE hSocket, const unsigned char * dataBuffer, int dataLength, int blockMaxSize /*= -1*/ )
{
    TRACE_SCOPE(areg_base_NESocketPosix_sendData);

    int result = -1;

    if ( hSocket != NESocket::InvalidSocketHandle )
    {
        result = 0;
        if ( dataBuffer != NULL && dataLength > 0 )
        {
            blockMaxSize    = blockMaxSize > 0 ? blockMaxSize : NESocket::getMaxSendSize(hSocket);
            result          = dataLength;

            TRACE_DBG("Going to send data, length = [ %d ], blockMaxSize [ %d ], socket is VALID", dataLength, blockMaxSize);
            while ( dataLength > 0 )
            {
                int remain = dataLength > blockMaxSize ? blockMaxSize : dataLength;
                TRACE_DBG("Sending [ %d ] bytes of data", remain);
                int written= send(hSocket, reinterpret_cast<const char *>(dataBuffer), remain, remain);
                if ( written > 0 )
                {
                    dataLength -= written;
                    dataBuffer += written;
                    TRACE_DBG("Has sent [ %d ] bytes of data, there is still [ %d ] bytes left", written, dataLength);
                }
                else
                {
                    if ( errno == EMSGSIZE )
                    {
                        // try again with other package size
                        blockMaxSize = NESocket::getMaxSendSize(hSocket);
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
            TRACE_ERR("Either buffer is null [ %s ] or wrong data length to sent [ %d ]", dataBuffer == NULL ? "YES" : "NO", dataLength);
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
        if ( dataBuffer != NULL && dataLength > 0 )
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
            TRACE_ERR("Either buffer is null [ %s ] or wrong data length to receive [ %d ]", dataBuffer == NULL ? "YES" : "NO", dataLength);
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

#endif  // _POSIX
