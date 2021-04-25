/************************************************************************
 * \file        areg/src/base/private/NESocketWin.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform. Socket OS independentant methods
 ************************************************************************/
#include "areg/src/base/NESocket.hpp"

#include "areg/src/base/GEMacros.h"
#include "areg/src/base/NEMemory.hpp"

#ifdef   _WINDOWS
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif  // WIN32_LEAN_AND_MEAN
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #include <sys/socket.h>
    #include <sys/select.h>
    #include <netinet/in.h>
    #include <netdb.h>
    #include <arpa/inet.h>
    #include <ctype.h>      // IEEE Std 1003.1-2001
#endif

//////////////////////////////////////////////////////////////////////////
// NESocket namespace members
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Constant, identifying invalid socket descriptor.
 **/
AREG_API const SOCKETHANDLE         NESocket::InvalidSocketHandle      = static_cast<SOCKETHANDLE>(~0);

/**
 * \brief   Constant, identifying invalid port number.
 **/
AREG_API const unsigned short       NESocket::InvalidPort               = 0;

/**
 * \brief   Constant, identifying local host
 **/
AREG_API const char * const         NESocket::LocalHost                 = "localhost";

/**
 * \brief   Constant, identifying maximum number of listeners in the queue. Used by server socket when set to listen connection.
 **/
AREG_API const int                  NESocket::MAXIMUM_LISTEN_QUEUE_SIZE = SOMAXCONN;

/**
 * \brief   The default size of segment when sends or receives data.
 **/
AREG_API const int                  NESocket::DEFAULT_SEGMENT_SIZE      = 16384;

//////////////////////////////////////////////////////////////////////////
// NESocket::CEInterlockedValue class implementation
//////////////////////////////////////////////////////////////////////////
NESocket::CEInterlockedValue::CEInterlockedValue( void )
    : mIpAddr   ( "" )
    , mPortNr   ( NESocket::InvalidPort )
{   ;   }

NESocket::CEInterlockedValue::CEInterlockedValue(const NESocket::CEInterlockedValue & src)
    : mIpAddr   ( src.mIpAddr )
    , mPortNr   ( src.mPortNr )
{   ;   }

const NESocket::CEInterlockedValue & NESocket::CEInterlockedValue::operator = ( const NESocket::CEInterlockedValue & src )
{
    if ( static_cast<const NESocket::CEInterlockedValue *>(this) != &src )
    {
        mIpAddr = src.mIpAddr;
        mPortNr = src.mPortNr;
    }
    return (*this);
}

bool NESocket::CEInterlockedValue::ConvertAddress(struct sockaddr_in & out_sockAddr) const
{
    bool result = false;
    if ( mPortNr != NESocket::InvalidPort )
    {
        NEMemory::ZeroBuffer(&out_sockAddr, sizeof(out_sockAddr));
        out_sockAddr.sin_family = AF_INET;
        out_sockAddr.sin_port   = htons( mPortNr );
        if (mIpAddr.IsEmpty() == false)
        {

#if     (_MSC_VER >= 1800)
            result = RETURNED_OK == inet_pton(AF_INET, mIpAddr.GetBuffer(), &out_sockAddr.sin_addr);
#else   // (_MSC_VER >= 1800)
            out_sockAddr.sin_addr.s_addr = inet_addr(mIpAddr.GetBuffer());
            result = true;
#endif  // (_MSC_VER >= 1800)

        }

        result = true;
    }

    return result;
}

bool NESocket::CEInterlockedValue::ResolveSocket(SOCKETHANDLE hSocket)
{
    bool result = false;
    mPortNr     = NESocket::InvalidPort;
    mIpAddr     = "";

    if ( hSocket != NESocket::InvalidSocketHandle )
    {
        // struct sockaddr sa  = {0};
        struct sockaddr_in sAddr;
        NEMemory::ZeroBuffer(&sAddr, sizeof(sockaddr));

        int len = sizeof(sockaddr);
        if ( RETURNED_OK == getpeername(hSocket, reinterpret_cast<struct sockaddr *>(&sAddr), &len) )
        {
            sockaddr_in & addr_in = reinterpret_cast<sockaddr_in &>(sAddr);
            if ( addr_in.sin_family == AF_INET )
            {
                mPortNr = ntohs( static_cast<uint16_t>(addr_in.sin_port) );

#if     (_MSC_VER >= 1800)
                char ipAddr[32] = { 0 };
                if ( RETURNED_OK == inet_ntop(AF_INET, &addr_in.sin_addr, ipAddr, 32) )
                {
                    mIpAddr = ipAddr;
                    result = true;
                }
#else   // (_MSC_VER >= 1800)
                mIpAddr = inet_ntoa(addr_in.sin_addr);
                result = true;
#endif  // (_MSC_VER >= 1800)

                OUTPUT_DBG("Succeeded to resolve name of socket [ %u ]. IP address = [ %s ], port number [ %u ]"
                            , static_cast<unsigned int>(hSocket)
                            , static_cast<const char *>(mIpAddr)
                            , static_cast<unsigned int>(mPortNr));
            }
        }
        else
        {
            OUTPUT_WARN("Failed to get peer name of socket [ %u ]", static_cast<unsigned int>(hSocket));
        }
    }
    else
    {
        OUTPUT_WARN("Invalid Socket Handle, cannot resolve name");
    }

    return result;
}

bool NESocket::CEInterlockedValue::ResolveAddress(const char * hostName, unsigned short portNr, bool isServer)
{
    bool result = false;
    hostName = hostName == NULL ? NESocket::LocalHost : hostName;

    mPortNr = NESocket::InvalidPort;
    mIpAddr = "";

    if ( isalnum(*hostName) )
    {
        // acquire address info
        char svcName[0x0F];
        CEString::PrintString(svcName, 0x0F, "%u", portNr);

        // struct addrinfo hints = {0};
        struct addrinfo hints;
        NEMemory::ZeroBuffer(&hints, sizeof(addrinfo));
        hints.ai_family     = AF_INET;
        hints.ai_socktype   = SOCK_STREAM;
        hints.ai_flags      = isServer ? AI_PASSIVE : 0;
        hints.ai_protocol   = IPPROTO_TCP;
        addrinfo * aiResult = NULL;

        if ( RETURNED_OK == getaddrinfo(hostName, static_cast<const char *>(svcName), &hints, &aiResult) )
        {
            ASSERT(aiResult != NULL);
            for ( addrinfo * addrInfo = aiResult; addrInfo != NULL; addrInfo = addrInfo->ai_next)
            {
                if ( addrInfo->ai_family == AF_INET && addrInfo->ai_socktype == SOCK_STREAM )
                {
                    struct sockaddr_in * addrIn = reinterpret_cast<struct sockaddr_in *>(addrInfo->ai_addr);
                    mPortNr = portNr;
#if     (_MSC_VER >= 1800)
                    char ipAddr[32] = { 0 };
                    if ( RETURNED_OK == inet_ntop(AF_INET, &addrIn->sin_addr, ipAddr, 32) )
                    {
                        mIpAddr = ipAddr;
                        result = true;
                    }
#else   // (_MSC_VER >= 1800)
                    mIpAddr = inet_ntoa(addrIn->sin_addr);
                    result = true;
#endif  // (_MSC_VER >= 1800)
                    OUTPUT_DBG("Succeeded to resolve name [ %s ] of [ %s ]. The IP address [ %s ], port number [ %u ]"
                                , hostName
                                , isServer ? "SERVER" : "CLIENT"
                                , static_cast<const char *>(mIpAddr)
                                , static_cast<unsigned int>(mPortNr));

                    break;
                }
            }

            freeaddrinfo( aiResult );
        }
        else
        {
            OUTPUT_ERR("Failed to resolve name [ %s ] and port number [ %u ] of [ %s ]", hostName, static_cast<unsigned int>(portNr), isServer ? "SERVER" : "CLIENT");
        }
    }
    else
    {
        OUTPUT_DBG("Ignored to resolve name for specified IP address [ %s ] and port number [ %u ] of [ %s ]", hostName, static_cast<unsigned int>(portNr), isServer ? "SERVER" : "CLIENT");
        mPortNr = portNr;
        mIpAddr = hostName;
        result  = true;
    }

    return result;
}

bool NESocket::CEInterlockedValue::operator == ( const NESocket::CEInterlockedValue & other ) const
{
    return (this != &other ? mIpAddr == other.mIpAddr && mPortNr == other.mPortNr : true);
}

bool NESocket::CEInterlockedValue::operator != ( const NESocket::CEInterlockedValue & other ) const
{
    return (this != &other ? mIpAddr != other.mIpAddr || mPortNr != other.mPortNr : false);
}

void NESocket::CEInterlockedValue::SetHostAddress(const struct sockaddr_in & addrHost)
{
    mPortNr = ntohs(addrHost.sin_port);
#if     (_MSC_VER >= 1800)
    char ipAddr[32] = { 0 };
    IN_ADDR & inAddr = const_cast<IN_ADDR &>(addrHost.sin_addr);
    if ( RETURNED_OK == inet_ntop(AF_INET, &inAddr, ipAddr, 32) )
    {
        mIpAddr = ipAddr;
    }
#else   // (_MSC_VER >= 1800)
    mIpAddr = inet_ntoa(addrHost.sin_addr);
#endif  // (_MSC_VER >= 1800)
}

//////////////////////////////////////////////////////////////////////////
// NESocket namespace functions implementation
//////////////////////////////////////////////////////////////////////////

AREG_API bool NESocket::IsSocketHandleValid(SOCKETHANDLE hSocket)
{
    return (hSocket != NESocket::InvalidSocketHandle);
}

AREG_API SOCKETHANDLE NESocket::SocketCreate( void )
{
    return static_cast<SOCKETHANDLE>( socket(AF_INET, SOCK_STREAM, IPPROTO_TCP) );
}

AREG_API int NESocket::GetMaximumSendSize( SOCKETHANDLE hSocket )
{
    long maxData= NESocket::DEFAULT_SEGMENT_SIZE;
    int temp    = sizeof(long);

    if ( (RETURNED_OK == getsockopt(hSocket, SOL_SOCKET, SO_SNDBUF, reinterpret_cast<char *>(&maxData), &temp))  || (maxData < 0) )
    {
        maxData = NESocket::DEFAULT_SEGMENT_SIZE;
    }

    return static_cast<int>(maxData);
}

AREG_API int NESocket::GetMaximumReceiveSize( SOCKETHANDLE hSocket )
{
    long maxData= NESocket::DEFAULT_SEGMENT_SIZE;
    int temp    = sizeof(long);

    if ( (RETURNED_OK == getsockopt(hSocket, SOL_SOCKET, SO_RCVBUF, reinterpret_cast<char *>(&maxData), &temp)) || (maxData < 0) )
    {
        maxData = NESocket::DEFAULT_SEGMENT_SIZE;
    }

    return static_cast<int>(maxData);
}

AREG_API SOCKETHANDLE NESocket::ClientSocketConnect(const char * hostName, unsigned short portNr, NESocket::CEInterlockedValue * out_socketAddr /*= NULL*/)
{
    hostName = hostName != NULL ? hostName : NESocket::LocalHost;

    OUTPUT_DBG("Creating client socket to connect remote host [ %s ] and port number [ %u ]", hostName, static_cast<unsigned int>(portNr));

    if ( out_socketAddr != NULL )
        out_socketAddr->ResetAddress();

    SOCKETHANDLE result   = NESocket::InvalidSocketHandle;
    NESocket::CEInterlockedValue sockAddress;
    if ( sockAddress.ResolveAddress(hostName, portNr, false) )
    {
        result = NESocket::ClientSocketConnect(sockAddress);
        if ( result != NESocket::InvalidSocketHandle && out_socketAddr != NULL )
            *out_socketAddr = sockAddress;
    }
    else
    {
        OUTPUT_ERR("Failed to resolve IP address for remote host name [ %s ] and port [ %u ], cannot create client socket", hostName, static_cast<unsigned int>(portNr));
    }

    return result;
}

AREG_API SOCKETHANDLE NESocket::ClientSocketConnect(const CEInterlockedValue & peerAddr)
{
    SOCKETHANDLE result   = NESocket::InvalidSocketHandle;
    if ( peerAddr.IsValid() )
    {
        // struct sockaddr_in remoteAddr = {0};
        sockaddr_in remoteAddr;
        VERIFY( peerAddr.ConvertAddress(remoteAddr) );
        result = NESocket::SocketCreate();
        if ( result != NESocket::InvalidSocketHandle )
        {
            if ( RETURNED_OK != connect(result, reinterpret_cast<sockaddr *>(&remoteAddr), sizeof(sockaddr_in)))
            {
                OUTPUT_ERR("Client failed to connect to remote host [ %s ] and port number [ %u ]. Closing socket [ %u ]"
                            , static_cast<const char *>(peerAddr.GetHostAddress())
                            , static_cast<unsigned int>(peerAddr.GetHostPort())
                            , static_cast<unsigned int>(result));

                NESocket::SocketClose(result);
                result = NESocket::InvalidSocketHandle;
            }
#ifdef DEBUG
            else
            {
                OUTPUT_DBG("Client socket [ %u ] succeeded to connect to remote host [ %s ] and port number [ %u ]"
                            , static_cast<unsigned int>(result)
                            , static_cast<const char *>(peerAddr.GetHostAddress())
                            , static_cast<unsigned int>(peerAddr.GetHostPort()));
            }
#endif  // DEBUG
        }
        else
        {
            OUTPUT_ERR("Failed to create socket, cannot create client!");
        }
    }
    else
    {
        OUTPUT_ERR("Address [ %s ] or port number [ %u ] is not valid. No client is created", static_cast<const char *>(peerAddr.GetHostAddress()), peerAddr.GetHostPort());
    }

    return result;
}

AREG_API SOCKETHANDLE NESocket::ServerSocketConnect(const char * hostName, unsigned short portNr, CEInterlockedValue * out_socketAddr /*= NULL */)
{
    hostName = hostName != NULL ? hostName : NESocket::LocalHost;

    OUTPUT_DBG("Creating server socket on host [ %s ] and port number [ %u ]", hostName, static_cast<unsigned int>(portNr));


    if ( out_socketAddr != NULL )
        out_socketAddr->ResetAddress();

    SOCKETHANDLE result   = NESocket::InvalidSocketHandle;
    NESocket::CEInterlockedValue sockAddress;
    if ( sockAddress.ResolveAddress(hostName, portNr, true) )
    {
        result = NESocket::ServerSocketConnect(sockAddress);
        if ( result != NESocket::InvalidSocketHandle && out_socketAddr != NULL )
            *out_socketAddr = sockAddress;
    }
    else
    {
        OUTPUT_ERR("Failed to resolve IP address for host name [ %s ] and port [ %u ], cannot create server socket", hostName, static_cast<unsigned int>(portNr));
    }

    return result;
}

AREG_API SOCKETHANDLE NESocket::ServerSocketConnect(const CEInterlockedValue & peerAddr)
{
    SOCKETHANDLE result   = NESocket::InvalidSocketHandle;
    if ( peerAddr.IsValid() )
    {
        // struct sockaddr_in remoteAddr = {0};
        sockaddr_in serverAddr;
        VERIFY( peerAddr.ConvertAddress(serverAddr) );
        result = NESocket::SocketCreate();
        if ( result != NESocket::InvalidSocketHandle )
        {
            int yes = 1;    // avoid the "address already in use" error message
            setsockopt( result, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char *>(&yes), sizeof(int) );
            if ( RETURNED_OK != bind(result, reinterpret_cast<sockaddr *>(&serverAddr), sizeof(sockaddr_in)) )
            {
                OUTPUT_ERR("Server failed to bind on host [ %s ] and port number [ %u ]. Closing socket [ %u ]"
                            , static_cast<const char *>(peerAddr.GetHostAddress())
                            , static_cast<unsigned int>(peerAddr.GetHostPort())
                            , static_cast<unsigned int>(result));

                NESocket::SocketClose( result );
                result = NESocket::InvalidSocketHandle;
            }
#ifdef  DEBUG
            else
            {
                OUTPUT_DBG("Server socket [ %u ] succeeded to bind on host [ %s ] and port number [ %u ]. Ready to listen."
                          , static_cast<unsigned int>(result)
                          , static_cast<const char *>(peerAddr.GetHostAddress())
                          , static_cast<unsigned int>(peerAddr.GetHostPort()));

            }
#endif  // DEBUG
        }
        else
        {
            OUTPUT_ERR("Failed to create socket, cannot create server!");
        }
    }
    else
    {
        OUTPUT_ERR("Address [ %s ] or port number [ %u ] is not valid. No server is created", static_cast<const char *>(peerAddr.GetHostAddress()), peerAddr.GetHostPort());
    }

    return result;
}

AREG_API bool NESocket::ServerListenConnection(SOCKETHANDLE serverSocket, int maxQueueSize /*= NESocket::MAXIMUM_LISTEN_QUEUE_SIZE*/)
{
    return ( (serverSocket != NESocket::InvalidSocketHandle) && (RETURNED_OK == listen(serverSocket, maxQueueSize)) );
}
