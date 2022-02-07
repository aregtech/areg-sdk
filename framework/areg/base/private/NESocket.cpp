/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/base/private/NESocketWin.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform. Socket OS independentant methods
 ************************************************************************/
#include "areg/base/NESocket.hpp"

#include "areg/base/GEMacros.h"
#include "areg/base/NEMemory.hpp"
#include "areg/trace/GETrace.h"

#ifdef   _WINDOWS
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif  // WIN32_LEAN_AND_MEAN
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #include <sys/socket.h>
    #include <sys/ioctl.h>
    #include <sys/select.h>
    #include <netinet/in.h>
    #include <netdb.h>
    #include <arpa/inet.h>
    #include <ctype.h>      // IEEE Std 1003.1-2001
#endif

#include <utility>

DEF_TRACE_SCOPE(areg_base_NESocket_clientSocketConnect);
DEF_TRACE_SCOPE(areg_base_NESocket_serverSocketConnect);
DEF_TRACE_SCOPE(areg_base_NESocket_serverAcceptConnection);

//////////////////////////////////////////////////////////////////////////
// NESocket namespace members
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Constant, identifying invalid socket descriptor.
 **/
AREG_API const SOCKETHANDLE         NESocket::InvalidSocketHandle      = static_cast<SOCKETHANDLE>(~0);

/**
 * \brief   Constant, identifying maximum number of listeners in the queue. Used by server socket when set to listen connection.
 **/
AREG_API const int                  NESocket::MAXIMUM_LISTEN_QUEUE_SIZE = SOMAXCONN;

//////////////////////////////////////////////////////////////////////////
// NESocket::SocketAddress class implementation
//////////////////////////////////////////////////////////////////////////
NESocket::SocketAddress::SocketAddress( void )
    : mIpAddr   ( "" )
    , mPortNr   ( NESocket::InvalidPort )
{
}

NESocket::SocketAddress::SocketAddress(const NESocket::SocketAddress & src)
    : mIpAddr   ( src.mIpAddr )
    , mPortNr   ( src.mPortNr )
{
}

NESocket::SocketAddress::SocketAddress( NESocket::SocketAddress && src ) noexcept
    : mIpAddr   ( std::move(src.mIpAddr) )
    , mPortNr   ( std::move(src.mPortNr) )
{
}

NESocket::SocketAddress & NESocket::SocketAddress::operator = ( const NESocket::SocketAddress & src )
{
    mIpAddr = src.mIpAddr;
    mPortNr = src.mPortNr;

    return (*this);
}

NESocket::SocketAddress & NESocket::SocketAddress::operator = ( NESocket::SocketAddress && src ) noexcept
{
    mIpAddr = std::move(src.mIpAddr);
    mPortNr = std::move(src.mPortNr);

    return (*this);
}

bool NESocket::SocketAddress::getAddress(struct sockaddr_in & out_sockAddr) const
{
    bool result = false;
    if ( mPortNr != NESocket::InvalidPort )
    {
        NEMemory::memZero(&out_sockAddr, sizeof(out_sockAddr));
        out_sockAddr.sin_family = AF_INET;
        out_sockAddr.sin_port   = htons( mPortNr );
        if (!mIpAddr.empty())
        {

#if defined(_MSC_VER) && (_MSC_VER >= 1800)
            result = RETURNED_OK == inet_pton(AF_INET, mIpAddr.c_str(), &out_sockAddr.sin_addr);
#else   // (_MSC_VER >= 1800)
            out_sockAddr.sin_addr.s_addr = inet_addr(mIpAddr.c_str());
            result = true;
#endif  // (_MSC_VER >= 1800)

        }

        result = true;
    }

    return result;
}

void NESocket::SocketAddress::setAddress(const struct sockaddr_in & addrHost)
{
    mPortNr = ntohs(addrHost.sin_port);
#if defined(_MSC_VER) && (_MSC_VER >= 1800)
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

bool NESocket::SocketAddress::resolveSocket(SOCKETHANDLE hSocket)
{
    bool result = false;
    mPortNr     = NESocket::InvalidPort;
    mIpAddr     = "";

    if ( hSocket != NESocket::InvalidSocketHandle )
    {
        // struct sockaddr sa  = {0};
        struct sockaddr_in sAddr;
        NEMemory::memZero(&sAddr, sizeof(sockaddr));

#ifdef  _WINDOWS
        int len = sizeof(sockaddr);
#else   // !_WINDOWS
        socklen_t len = sizeof(sockaddr);
#endif  // _WINDOWS

        if ( RETURNED_OK == getpeername(hSocket, reinterpret_cast<struct sockaddr *>(&sAddr), &len) )
        {
            sockaddr_in & addr_in = reinterpret_cast<sockaddr_in &>(sAddr);
            if ( addr_in.sin_family == AF_INET )
            {
                mPortNr = ntohs( static_cast<uint16_t>(addr_in.sin_port) );

#if defined(_MSC_VER) && (_MSC_VER >= 1800)
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

bool NESocket::SocketAddress::resolveAddress( const std::string_view & hostName, unsigned short portNr, bool isServer)
{
    bool result = false;
    
    const char * host = hostName.empty() ? NESocket::LocalHost.data() : hostName.data();

    mPortNr = NESocket::InvalidPort;
    mIpAddr = "";

    if ( isalnum(*host) )
    {
        // acquire address info
        char svcName[0x0F];
        String::formatString(svcName, 0x0F, "%u", portNr);

        // struct addrinfo hints = {0};
        struct addrinfo hints;
        NEMemory::memZero(&hints, sizeof(addrinfo));
        hints.ai_family     = AF_INET;
        hints.ai_socktype   = SOCK_STREAM;
        hints.ai_flags      = isServer ? AI_PASSIVE : 0;
        hints.ai_protocol   = IPPROTO_TCP;
        addrinfo * aiResult = nullptr;

        if ( RETURNED_OK == getaddrinfo(host, static_cast<const char *>(svcName), &hints, &aiResult) )
        {
            ASSERT(aiResult != nullptr);
            for ( addrinfo * addrInfo = aiResult; addrInfo != nullptr; addrInfo = addrInfo->ai_next)
            {
                if ( addrInfo->ai_family == AF_INET && addrInfo->ai_socktype == SOCK_STREAM )
                {
                    struct sockaddr_in * addrIn = reinterpret_cast<struct sockaddr_in *>(addrInfo->ai_addr);
                    mPortNr = portNr;
#if defined(_MSC_VER) && (_MSC_VER >= 1800)
                    char ipAddr[32] = { 0 };
                    mIpAddr = inet_ntop( AF_INET, &addrIn->sin_addr, ipAddr, 32 );
                    result  = mIpAddr.isEmpty() == false;
#else   // (_MSC_VER >= 1800)
                    mIpAddr = inet_ntoa(addrIn->sin_addr);
                    result = true;
#endif  // (_MSC_VER >= 1800)
                    OUTPUT_DBG("Succeeded to resolve name [ %s ] of [ %s ]. The IP address [ %s ], port number [ %u ]"
                                , host
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
            OUTPUT_ERR("Failed to resolve name [ %s ] and port number [ %u ] of [ %s ]", host, static_cast<unsigned int>(portNr), isServer ? "SERVER" : "CLIENT");
        }
    }
    else
    {
        OUTPUT_DBG("Ignored to resolve name for specified IP address [ %s ] and port number [ %u ] of [ %s ]", host, static_cast<unsigned int>(portNr), isServer ? "SERVER" : "CLIENT");
        mPortNr = portNr;
        mIpAddr = host;
        result  = true;
    }

    return result;
}

bool NESocket::SocketAddress::operator == ( const NESocket::SocketAddress & other ) const
{
    return (this != &other ? mIpAddr == other.mIpAddr && mPortNr == other.mPortNr : true);
}

bool NESocket::SocketAddress::operator != ( const NESocket::SocketAddress & other ) const
{
    return (this != &other ? mIpAddr != other.mIpAddr || mPortNr != other.mPortNr : false);
}

//////////////////////////////////////////////////////////////////////////
// NESocket namespace functions implementation
//////////////////////////////////////////////////////////////////////////

AREG_API bool NESocket::isSocketHandleValid(SOCKETHANDLE hSocket)
{
    return (hSocket != NESocket::InvalidSocketHandle);
}

AREG_API SOCKETHANDLE NESocket::socketCreate( void )
{
    return static_cast<SOCKETHANDLE>( socket(AF_INET, SOCK_STREAM, IPPROTO_TCP) );
}

AREG_API int NESocket::getMaxSendSize( SOCKETHANDLE hSocket )
{
    long maxData= NESocket::DEFAULT_SEGMENT_SIZE;

#ifdef  _WINDOWS
        int len = sizeof(long);
#else   // !_WINDOWS
        socklen_t len = sizeof(long);
#endif  // _WINDOWS

    if ( (RETURNED_OK == getsockopt(hSocket, SOL_SOCKET, SO_SNDBUF, reinterpret_cast<char *>(&maxData), &len))  || (maxData < 0) )
    {
        maxData = NESocket::DEFAULT_SEGMENT_SIZE;
    }

    return static_cast<int>(maxData);
}

AREG_API int NESocket::getMaxReceiveSize( SOCKETHANDLE hSocket )
{
    long maxData= NESocket::DEFAULT_SEGMENT_SIZE;

#ifdef  _WINDOWS
        int len = sizeof(long);
#else   // !_WINDOWS
        socklen_t len = sizeof(long);
#endif  // _WINDOWS

    if ( (RETURNED_OK == getsockopt(hSocket, SOL_SOCKET, SO_RCVBUF, reinterpret_cast<char *>(&maxData), &len)) || (maxData < 0) )
    {
        maxData = NESocket::DEFAULT_SEGMENT_SIZE;
    }

    return static_cast<int>(maxData);
}

AREG_API SOCKETHANDLE NESocket::clientSocketConnect(const std::string_view & hostName, unsigned short portNr, NESocket::SocketAddress * out_socketAddr /*= nullptr*/)
{
    TRACE_SCOPE(areg_base_NESocket_clientSocketConnect);

    const char * host = hostName.empty() ? hostName.data() : NESocket::LocalHost.data();

    TRACE_DBG("Creating client socket to connect remote host [ %s ] and port number [ %u ]", host, static_cast<unsigned int>(portNr));

    if ( out_socketAddr != nullptr )
        out_socketAddr->resetAddress();

    SOCKETHANDLE result   = NESocket::InvalidSocketHandle;
    NESocket::SocketAddress sockAddress;
    if ( sockAddress.resolveAddress(host, portNr, false) )
    {
        result = NESocket::clientSocketConnect(sockAddress);
        if ( result != NESocket::InvalidSocketHandle && out_socketAddr != nullptr )
            *out_socketAddr = sockAddress;
    }
    else
    {
        TRACE_ERR("FAILED to resolve IP address for remote host name [ %s ] and port [ %u ], cannot create client socket", host, static_cast<unsigned int>(portNr));
    }

    return result;
}

AREG_API SOCKETHANDLE NESocket::clientSocketConnect(const SocketAddress & peerAddr)
{
    TRACE_SCOPE(areg_base_NESocket_clientSocketConnect);

    SOCKETHANDLE result   = NESocket::InvalidSocketHandle;
    if ( peerAddr.isValid() )
    {
        // struct sockaddr_in remoteAddr = {0};
        sockaddr_in remoteAddr;
        VERIFY( peerAddr.getAddress(remoteAddr) );
        result = NESocket::socketCreate();
        if ( result != NESocket::InvalidSocketHandle )
        {
            if ( RETURNED_OK != connect(result, reinterpret_cast<sockaddr *>(&remoteAddr), sizeof(sockaddr_in)))
            {
                TRACE_ERR("Client failed to connect to remote host [ %s ] and port number [ %u ]. Closing socket [ %u ]"
                            , peerAddr.getHostAddress().c_str()
                            , static_cast<unsigned int>(peerAddr.getHostPort())
                            , static_cast<unsigned int>(result));

                NESocket::socketClose(result);
                result = NESocket::InvalidSocketHandle;
            }
#ifdef DEBUG
            else
            {
                TRACE_DBG("Client socket [ %u ] succeeded to connect to remote host [ %s ] and port number [ %u ]"
                            , static_cast<unsigned int>(result)
                            , static_cast<const char *>(peerAddr.getHostAddress())
                            , static_cast<unsigned int>(peerAddr.getHostPort()));
            }
#endif  // DEBUG
        }
        else
        {
            TRACE_ERR("Failed to create socket, cannot create client!");
        }
    }
    else
    {
        TRACE_ERR("Address [ %s ] or port number [ %u ] is not valid. No client is created", peerAddr.getHostAddress().c_str(), peerAddr.getHostPort());
    }

    return result;
}

AREG_API SOCKETHANDLE NESocket::serverSocketConnect(const std::string_view & hostName, unsigned short portNr, SocketAddress * out_socketAddr /*= nullptr */)
{
    TRACE_SCOPE(areg_base_NESocket_serverSocketConnect);

    const char * host = hostName.empty() ? hostName.data() : NESocket::LocalHost.data();

    TRACE_DBG("Creating server socket on host [ %s ] and port number [ %u ]", host, static_cast<unsigned int>(portNr));


    if ( out_socketAddr != nullptr )
        out_socketAddr->resetAddress();

    SOCKETHANDLE result   = NESocket::InvalidSocketHandle;
    NESocket::SocketAddress sockAddress;
    if ( sockAddress.resolveAddress(host, portNr, true) )
    {
        result = NESocket::serverSocketConnect(sockAddress);
        if ( result != NESocket::InvalidSocketHandle && out_socketAddr != nullptr )
            *out_socketAddr = sockAddress;
    }
    else
    {
        TRACE_ERR("FAILED to resolve IP address for host name [ %s ] and port [ %u ], cannot create server socket", host, static_cast<unsigned int>(portNr));
    }

    return result;
}

AREG_API SOCKETHANDLE NESocket::serverSocketConnect(const SocketAddress & peerAddr)
{
    TRACE_SCOPE(areg_base_NESocket_serverSocketConnect);

    SOCKETHANDLE result   = NESocket::InvalidSocketHandle;
    if ( peerAddr.isValid() )
    {
        // struct sockaddr_in remoteAddr = {0};
        sockaddr_in serverAddr;
        VERIFY( peerAddr.getAddress(serverAddr) );
        result = NESocket::socketCreate();
        if ( result != NESocket::InvalidSocketHandle )
        {
            int yes = 1;    // avoid the "address already in use" error message
            setsockopt( result, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char *>(&yes), sizeof(int) );
            if ( RETURNED_OK != bind(result, reinterpret_cast<sockaddr *>(&serverAddr), sizeof(sockaddr_in)) )
            {
                TRACE_ERR("Server failed to bind on host [ %s ] and port number [ %u ]. Closing socket [ %u ]"
                            , peerAddr.getHostAddress().c_str()
                            , static_cast<unsigned int>(peerAddr.getHostPort())
                            , static_cast<unsigned int>(result));

                NESocket::socketClose( result );
                result = NESocket::InvalidSocketHandle;
            }
#ifdef  DEBUG
            else
            {
                TRACE_DBG("Server socket [ %u ] succeeded to bind on host [ %s ] and port number [ %u ]. Ready to listen."
                          , static_cast<unsigned int>(result)
                          , peerAddr.getHostAddress().c_str()
                          , static_cast<unsigned int>(peerAddr.getHostPort()));

            }
#endif  // DEBUG
        }
        else
        {
            TRACE_ERR("Failed to create socket, cannot create server!");
        }
    }
    else
    {
        TRACE_ERR("Address [ %s ] or port number [ %u ] is not valid. No server is created", peerAddr.getHostAddress().c_str(), peerAddr.getHostPort());
    }

    return result;
}

AREG_API bool NESocket::serverListenConnection(SOCKETHANDLE serverSocket, int maxQueueSize /*= NESocket::MAXIMUM_LISTEN_QUEUE_SIZE*/)
{
    return ( (serverSocket != NESocket::InvalidSocketHandle) && (RETURNED_OK == listen(serverSocket, maxQueueSize)) );
}

AREG_API SOCKETHANDLE NESocket::serverAcceptConnection(SOCKETHANDLE serverSocket, const SOCKETHANDLE * masterList, int entriesCount, NESocket::SocketAddress * out_socketAddr /*= nullptr*/)
{
    TRACE_SCOPE(areg_base_NESocket_serverAcceptConnection);

    TRACE_DBG("Checking server socket event, server socket handle [ %u ]", static_cast<unsigned int>(serverSocket));

    if (out_socketAddr != nullptr )
        out_socketAddr->resetAddress();

    SOCKETHANDLE result = NESocket::InvalidSocketHandle;
    if ( serverSocket != NESocket::InvalidSocketHandle )
    {
        fd_set readList;
        FD_ZERO(&readList);
        FD_SET( serverSocket, &readList );
        SOCKETHANDLE maxSocket = serverSocket;

        if ( (masterList != nullptr) && (entriesCount > 0) )
        {
            entriesCount= MACRO_MIN(entriesCount, (FD_SETSIZE - 1));

#ifdef  WINDOWS

            for ( int count = 0; count < entriesCount; ++ count)
                readList.fd_array[count + 1] = masterList[count];
            
            readList.fd_count = static_cast<u_int>( entriesCount + 1 );

#else   // !WINDOWS

            TRACE_DBG("There are [ %d ] socket entries in the master list, setting FD_SET", entriesCount);
            for ( int count = 0; count < entriesCount; ++ count)
            {
                SOCKETHANDLE sh = masterList[count];
                if ( NESocket::isSocketAlive(sh))
                {
                    FD_SET(masterList[count], &readList);
                    maxSocket = MACRO_MAX(maxSocket, sh);
                }
                else
                {
                    TRACE_WARN("Found socket [ %u ] that is not alive anymore, break accepting connection to handle error.", ((unsigned int)sh));
                    result = sh;
                    break;
                }
            }

#endif  // WINDOWS
        }

        if (result == NESocket::InvalidSocketHandle)
        {
            TRACE_DBG("Call select to wait socket connection, max socket value is [ %d ]", static_cast<int>(maxSocket));
            int selected    = select( static_cast<int>(maxSocket) + 1 /* param is ignored in Win32*/, &readList, nullptr, nullptr, nullptr);
            if ( selected > 0 )
            {
                if ( FD_ISSET(serverSocket, &readList) != 0 )
                {
                    // have got new client connection. resolve and get socket
                    struct sockaddr_in acceptAddr; // connecting client address information
                    NEMemory::memZero(&acceptAddr, sizeof(sockaddr_in));

#ifdef  _WINDOWS
                    int len = sizeof(sockaddr_in);
#else   // !_WINDOWS
                    socklen_t len = sizeof(sockaddr_in);
#endif  // _WINDOWS

                    TRACE_DBG("... server waiting for new connection event ...");
                    result = accept( serverSocket, reinterpret_cast<sockaddr *>(&acceptAddr), &len );
                    TRACE_DBG("Server accepted new connection of client socket [ %u ]", static_cast<unsigned int>(result));
                    if ( (result != NESocket::InvalidSocketHandle) && (out_socketAddr != nullptr) )
                        out_socketAddr->setAddress(acceptAddr);
                }
                else
                {
                    TRACE_DBG("Have got select event of existing connection, going to resolve socket");

                    //  check whether have got connection from existing clients. if 'yes', server can read data.
                    for ( int count = 0; result == NESocket::InvalidSocketHandle && count < entriesCount; ++ count )
                    {
                        if (FD_ISSET( masterList[count], &readList ) != 0)
                        {
                            result = masterList[count];
                            TRACE_DBG("Server selected event of existing client socket [ %u ] connection", static_cast<unsigned int>(result));
                            break;
                        }
                    }
                }
            }
            else
            {
                TRACE_ERR("Failed to select connection. The server socket [ %u ] might be closed and not valid anymore, return value [ %d ]", static_cast<unsigned int>(serverSocket), selected);
            }
        }
        else
        {
            TRACE_ERR("Invalid server socket, ignoring accept connections!");
        }
    }
    else
    {
        TRACE_WARN("Found broken connection of socket [ %u ]", ((unsigned int)result));
        return result;
    }

    return result;
}

AREG_API bool NESocket::isSocketAlive(SOCKETHANDLE hSocket)
{
    bool result = false;

    int error = 0;
    socklen_t len = sizeof (error);

#ifdef _WINDOWS
    result = (getsockopt(static_cast<SOCKET>(hSocket), SOL_SOCKET, SO_ERROR, reinterpret_cast<char *>(&error), &len) == 0) && (error == 0);
#else
    result = (getsockopt(static_cast<int>(hSocket), SOL_SOCKET, SO_ERROR, reinterpret_cast<char *>(&error), &len) == 0) && (error == 0);
#endif

    return result;
}

AREG_API int NESocket::pendingRead(SOCKETHANDLE hSocket)
{
#ifdef _WINDOWS
    unsigned long result = 0;
    return (ioctlsocket(static_cast<SOCKET>(hSocket), FIONREAD, &result) == 0 ? static_cast<int>(result) : -1);
#else   // _WINDOWS
    int result = 0;
    return (ioctl(static_cast<int>(hSocket), FIONREAD, &result) == 0 ? result : -1);
#endif  // _WINDOWS

    return static_cast<int>(result);
}
