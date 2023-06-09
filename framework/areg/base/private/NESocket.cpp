/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
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

namespace NESocket
{
    // OS specific methods

    /**
     * \brief   OS specific socket initialization. Required in Win32 to initialize resources.
     * \return  Returns true if initialization succeeded.
     **/
    bool _osInitSocket(void);

    /**
     * \brief   OS specific socket release. Required in Win32 to release resources.
     */
    void _osReleaseSocket(void);

    /**
     * \brief   OS specific socket close.
     */
    void _osCloseSocket(SOCKETHANDLE hSocket);

    /**
     * \brief   OS specific send data implementation. All checkups and validations should
     *          be done before calling the method.
     * \return  Returns number of bytes sent via network.
     */
    int _osSendData(SOCKETHANDLE hSocket, const unsigned char* dataBuffer, int dataLength, int blockMaxSize);

    /**
     * \brief   OS specific receive data implementation. All checkups and validations should
     *          be done before calling the method.
     * \return  Returns number of bytes received via network.
     */
    int _osRecvData(SOCKETHANDLE hSocket, unsigned char* dataBuffer, int dataLength, int blockMaxSize);

    /**
     * \brief   OS specific implementation of socket control call.
     * \return  Returns true if operation succeeded.
     */
    bool _osControl(SOCKETHANDLE hSocket, int cmd, unsigned long & arg);

    /**
     * \brief   OS specific implementation of retrieving socket option.
     *          On output the 'value' indicates the value of the option,
     *          which is valid only if function returns true.
     */
    bool _osGetOption(SOCKETHANDLE hSocket, int level, int name, unsigned long & value);
}

DEF_TRACE_SCOPE(areg_base_NESocket_clientSocketConnect);
DEF_TRACE_SCOPE(areg_base_NESocket_serverSocketConnect);
DEF_TRACE_SCOPE(areg_base_NESocket_serverAcceptConnection);

//////////////////////////////////////////////////////////////////////////
// NESocket namespace members
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Constant, identifying maximum number of listeners in the queue. Used by server socket when set to listen connection.
 **/
AREG_API_IMPL const int NESocket::MAXIMUM_LISTEN_QUEUE_SIZE = SOMAXCONN;

//////////////////////////////////////////////////////////////////////////
// NESocket::SocketAddress class implementation
//////////////////////////////////////////////////////////////////////////
NESocket::SocketAddress::SocketAddress( void )
    : mIpAddr   ( )
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
        if (mIpAddr.isEmpty() == false)
        {

#if defined(_MSC_VER) && (_MSC_VER >= 1800)

            result = 1 == ::inet_pton(AF_INET, mIpAddr.getString(), &out_sockAddr.sin_addr);

#else   // (_MSC_VER >= 1800) || POSIX

            out_sockAddr.sin_addr.s_addr = ::inet_addr(mIpAddr.getString());
            result = true;

#endif  // (_MSC_VER >= 1800) || POSIX

        }
    }

    return result;
}

void NESocket::SocketAddress::setAddress(const struct sockaddr_in & addrHost)
{
    mPortNr = ntohs(addrHost.sin_port);

#if defined(_MSC_VER) && (_MSC_VER >= 1800)

    char ipAddr[32] = { 0 };
    IN_ADDR & inAddr = const_cast<IN_ADDR &>(addrHost.sin_addr);
    if ( nullptr != inet_ntop(AF_INET, &inAddr, ipAddr, 32) )
    {
        mIpAddr = ipAddr;
    }

#else   // (_MSC_VER >= 1800) || POSIX

    mIpAddr = inet_ntoa(addrHost.sin_addr);

#endif  // (_MSC_VER >= 1800) || POSIX
}

bool NESocket::SocketAddress::resolveSocket(SOCKETHANDLE hSocket)
{
    bool result = false;
    mPortNr     = NESocket::InvalidPort;
    mIpAddr     = "";

    if ( hSocket != NESocket::InvalidSocketHandle )
    {
        struct sockaddr_in sAddr;
        NEMemory::memZero(&sAddr, sizeof(sockaddr));

        socklen_t len = sizeof(sockaddr);
        if ( RETURNED_OK == ::getpeername(hSocket, reinterpret_cast<struct sockaddr *>(&sAddr), &len) )
        {
            sockaddr_in & addr_in = reinterpret_cast<sockaddr_in &>(sAddr);
            if ( addr_in.sin_family == AF_INET )
            {
                setAddress(addr_in);
                result = true;

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

    if ( ::isalnum(*host) )
    {
        // acquire address info
        char svcName[0x0F];
        String::formatString(svcName, 0x0F, "%u", portNr);

        struct addrinfo hints;
        NEMemory::memZero(&hints, sizeof(addrinfo));
        hints.ai_family     = AF_INET;
        hints.ai_socktype   = SOCK_STREAM;
        hints.ai_flags      = isServer ? AI_PASSIVE : 0;
        hints.ai_protocol   = IPPROTO_TCP;
        addrinfo * aiResult = nullptr;

        if ( RETURNED_OK == ::getaddrinfo(host, static_cast<const char *>(svcName), &hints, &aiResult) )
        {
            ASSERT(aiResult != nullptr);
            for ( addrinfo * addrInfo = aiResult; addrInfo != nullptr; addrInfo = addrInfo->ai_next)
            {
                if ( addrInfo->ai_family == AF_INET && addrInfo->ai_socktype == SOCK_STREAM )
                {
                    struct sockaddr_in * addrIn = reinterpret_cast<struct sockaddr_in *>(addrInfo->ai_addr);
                    setAddress(*addrIn);
                    result = mIpAddr.isEmpty() == false;
                    mPortNr = portNr;

                    OUTPUT_DBG("Succeeded to resolve name [ %s ] of [ %s ]. The IP address [ %s ], port number [ %u ]"
                                , host
                                , isServer ? "SERVER" : "CLIENT"
                                , static_cast<const char *>(mIpAddr)
                                , static_cast<unsigned int>(mPortNr));

                    break;
                }
            }

            ::freeaddrinfo( aiResult );
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

AREG_API_IMPL bool NESocket::isSocketHandleValid(SOCKETHANDLE hSocket)
{
    return (hSocket != NESocket::InvalidSocketHandle);
}

AREG_API_IMPL SOCKETHANDLE NESocket::socketCreate( void )
{
    return static_cast<SOCKETHANDLE>( socket(AF_INET, SOCK_STREAM, IPPROTO_TCP) );
}

AREG_API_IMPL unsigned int NESocket::getMaxSendSize( SOCKETHANDLE hSocket )
{
    ASSERT(hSocket != NESocket::InvalidSocketHandle);

    unsigned long maxData= NESocket::DEFAULT_SEGMENT_SIZE;
    return (_osGetOption(hSocket, SOL_SOCKET, SO_SNDBUF, maxData) ? static_cast<unsigned int>(maxData) : NESocket::DEFAULT_SEGMENT_SIZE);
}

AREG_API_IMPL unsigned int NESocket::setMaxSendSize(SOCKETHANDLE hSocket, unsigned int sendSize)
{
    ASSERT(hSocket != NESocket::InvalidSocketHandle);

    if (sendSize == 0)
    {
        sendSize = NESocket::DEFAULT_SEGMENT_SIZE;
    }
    else if (sendSize < NESocket::MIN_SEGMENT_SIZE)
    {
        sendSize = NESocket::MIN_SEGMENT_SIZE;
    }
    else if (sendSize > NESocket::MAX_SEGMENT_SIZE)
    {
        sendSize = NESocket::MAX_SEGMENT_SIZE;
    }

    return (RETURNED_OK == ::setsockopt(hSocket, SOL_SOCKET, SO_SNDBUF, reinterpret_cast<const char*>(&sendSize), sizeof(sendSize)) ? sendSize : NESocket::MIN_SEGMENT_SIZE);
}

AREG_API_IMPL unsigned int NESocket::getMaxReceiveSize( SOCKETHANDLE hSocket )
{
    ASSERT(hSocket != NESocket::InvalidSocketHandle);
    unsigned long maxData = NESocket::DEFAULT_SEGMENT_SIZE;
    return (_osGetOption(hSocket, SOL_SOCKET, SO_RCVBUF, maxData) ? static_cast<unsigned int>(maxData) : NESocket::DEFAULT_SEGMENT_SIZE);
}

AREG_API_IMPL unsigned int NESocket::setMaxReceiveSize(SOCKETHANDLE hSocket, unsigned int recvSize)
{
    ASSERT(hSocket != NESocket::InvalidSocketHandle);

    if (recvSize == 0)
    {
        recvSize = NESocket::DEFAULT_SEGMENT_SIZE;
    }
    else if (recvSize < NESocket::MIN_SEGMENT_SIZE)
    {
        recvSize = NESocket::MIN_SEGMENT_SIZE;
    }
    else if (recvSize > NESocket::MAX_SEGMENT_SIZE)
    {
        recvSize = NESocket::MAX_SEGMENT_SIZE;
    }

    return (RETURNED_OK == ::setsockopt(hSocket, SOL_SOCKET, SO_RCVBUF, reinterpret_cast<const char*>(&recvSize), sizeof(recvSize)) ? recvSize : NESocket::MIN_SEGMENT_SIZE);
}

AREG_API_IMPL SOCKETHANDLE NESocket::clientSocketConnect(const std::string_view & hostName, unsigned short portNr, NESocket::SocketAddress * out_socketAddr /*= nullptr*/)
{
    TRACE_SCOPE(areg_base_NESocket_clientSocketConnect);

    const char * host = hostName.empty() ? hostName.data() : NESocket::LocalHost.data();

    TRACE_DBG("Creating client socket to connect remote host [ %s ] and port number [ %u ]", host, static_cast<unsigned int>(portNr));

    if (out_socketAddr != nullptr)
    {
        out_socketAddr->resetAddress();
    }

    SOCKETHANDLE result   = NESocket::InvalidSocketHandle;
    NESocket::SocketAddress sockAddress;
    if ( sockAddress.resolveAddress(host, portNr, false) )
    {
        result = NESocket::clientSocketConnect(sockAddress);
        if ((result != NESocket::InvalidSocketHandle) && (out_socketAddr != nullptr))
        {
            *out_socketAddr = sockAddress;
        }
    }
    else
    {
        TRACE_ERR("FAILED to resolve IP address for remote host name [ %s ] and port [ %u ], cannot create client socket", host, static_cast<unsigned int>(portNr));
    }

    return result;
}

AREG_API_IMPL SOCKETHANDLE NESocket::clientSocketConnect(const SocketAddress & peerAddr)
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
                            , static_cast<const char *>(peerAddr.getHostAddress())
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
        TRACE_ERR("Address [ %s ] or port number [ %u ] is not valid. No client is created", static_cast<const char *>(peerAddr.getHostAddress()), peerAddr.getHostPort());
    }

    return result;
}

AREG_API_IMPL SOCKETHANDLE NESocket::serverSocketConnect(const std::string_view & hostName, unsigned short portNr, SocketAddress * out_socketAddr /*= nullptr */)
{
    TRACE_SCOPE(areg_base_NESocket_serverSocketConnect);

    const char * host = hostName.empty() ? hostName.data() : NESocket::LocalHost.data();

    TRACE_DBG("Creating server socket on host [ %s ] and port number [ %u ]", host, static_cast<unsigned int>(portNr));


    if (out_socketAddr != nullptr)
    {
        out_socketAddr->resetAddress();
    }

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

AREG_API_IMPL SOCKETHANDLE NESocket::serverSocketConnect(const SocketAddress & peerAddr)
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
            ::setsockopt( result, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char *>(&yes), sizeof(int) );
            if ( RETURNED_OK != bind(result, reinterpret_cast<sockaddr *>(&serverAddr), sizeof(sockaddr_in)) )
            {
                TRACE_ERR("Server failed to bind on host [ %s ] and port number [ %u ]. Closing socket [ %u ]"
                            , static_cast<const char *>(peerAddr.getHostAddress())
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
                          , static_cast<const char *>(peerAddr.getHostAddress())
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
        TRACE_ERR("Address [ %s ] or port number [ %u ] is not valid. No server is created", static_cast<const char *>(peerAddr.getHostAddress()), peerAddr.getHostPort());
    }

    return result;
}

AREG_API_IMPL bool NESocket::serverListenConnection(SOCKETHANDLE serverSocket, int maxQueueSize /*= NESocket::MAXIMUM_LISTEN_QUEUE_SIZE*/)
{
    return ( (serverSocket != NESocket::InvalidSocketHandle) && (RETURNED_OK == listen(serverSocket, maxQueueSize)) );
}

AREG_API_IMPL SOCKETHANDLE NESocket::serverAcceptConnection(SOCKETHANDLE serverSocket, const SOCKETHANDLE * masterList, int entriesCount, NESocket::SocketAddress * out_socketAddr /*= nullptr*/)
{
    TRACE_SCOPE(areg_base_NESocket_serverAcceptConnection);
    TRACE_DBG("Checking server socket event, server socket handle [ %u ]", static_cast<unsigned int>(serverSocket));

    SOCKETHANDLE result = NESocket::InvalidSocketHandle;
    if (masterList == nullptr)
    {
        TRACE_ERR("Invalid list of sockets, cannot accept connection");
        return result;
    }

    if (out_socketAddr != nullptr)
    {
        out_socketAddr->resetAddress();
    }

    if ( serverSocket != NESocket::InvalidSocketHandle )
    {
        fd_set readList;
        FD_ZERO(&readList);
        FD_SET( serverSocket, &readList );
        SOCKETHANDLE maxSocket = serverSocket;

        if ( entriesCount > 0 )
        {
            entriesCount= MACRO_MIN(entriesCount, (FD_SETSIZE - 1));

#ifdef  _WINDOWS

            for ( int count = 0; count < entriesCount; ++ count)
                readList.fd_array[count + 1] = masterList[count];
            
            readList.fd_count = static_cast<u_int>( entriesCount + 1 );

#else   // !_WINDOWS

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

#endif  // !_WINDOWS
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

                    socklen_t len = sizeof(sockaddr_in);
                    TRACE_DBG("... server waiting for new connection event ...");
                    result = ::accept( serverSocket, reinterpret_cast<sockaddr *>(&acceptAddr), &len );
                    TRACE_DBG("Server accepted new connection of client socket [ %u ]", static_cast<unsigned int>(result));
                    if ((result != NESocket::InvalidSocketHandle) && (out_socketAddr != nullptr))
                    {
                        out_socketAddr->setAddress(acceptAddr);
                    }
                }
                else
                {
                    TRACE_DBG("Have got select event of existing connection, going to resolve socket");

                    //  check whether have got connection from existing clients. if 'yes', server can read data.
                    for ( int count = 0; count < entriesCount; ++ count )
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
                result = NESocket::FailedSocketHandle;
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

AREG_API_IMPL bool NESocket::isSocketAlive(SOCKETHANDLE hSocket)
{
    unsigned long error = 0;
    return ((hSocket != NESocket::InvalidSocketHandle) && _osGetOption(hSocket, SOL_SOCKET, SO_ERROR, error) && (error == 0));
}

AREG_API_IMPL unsigned int NESocket::pendingRead(SOCKETHANDLE hSocket)
{
    unsigned long result = 0;
    return ((hSocket != NESocket::InvalidSocketHandle) && _osControl(hSocket, FIONREAD, result) ? static_cast<unsigned int>(result) : 0);
}

AREG_API_IMPL bool NESocket::socketInitialize(void)
{
    return _osInitSocket();
}

AREG_API_IMPL void NESocket::socketRelease(void)
{
    _osReleaseSocket();
}

AREG_API_IMPL void NESocket::socketClose(SOCKETHANDLE hSocket)
{
    if (hSocket != NESocket::InvalidSocketHandle)
    {
        _osCloseSocket(hSocket);
    }
}

AREG_API_IMPL int NESocket::sendData(SOCKETHANDLE hSocket, const unsigned char* dataBuffer, int dataLength, int blockMaxSize /*= NECommon::DEFAULT_SIZE*/)
{
    int result = -1;
    if (hSocket != NESocket::InvalidSocketHandle)
    {
        result = 0;
        if ((dataBuffer != nullptr) && (dataLength > 0))
        {
            result = _osSendData(hSocket, dataBuffer, dataLength, blockMaxSize > 0 ? blockMaxSize : NESocket::getMaxSendSize(hSocket));
        }
    }

    return result;
}

AREG_API_IMPL int NESocket::receiveData(SOCKETHANDLE hSocket, unsigned char* dataBuffer, int dataLength, int blockMaxSize )
{
    int result = -1;

    if (hSocket != NESocket::InvalidSocketHandle)
    {
        result = 0;
        if ((dataBuffer != nullptr) && (dataLength > 0))
        {
            result = _osRecvData(hSocket, dataBuffer, dataLength, blockMaxSize > 0 ? blockMaxSize : NESocket::getMaxReceiveSize(hSocket));
        }
    }

    return result;
}

AREG_API_IMPL bool NESocket::disableSend(SOCKETHANDLE hSocket)
{
#ifdef WINDOWS
    int flag{ SD_SEND };
#else
    int flag{ SHUT_WR };
#endif // WINDOWS

    return (hSocket != NESocket::InvalidSocketHandle ? RETURNED_OK == ::shutdown(hSocket, flag) : false);
}

AREG_API_IMPL bool NESocket::disableReceive(SOCKETHANDLE hSocket)
{
#ifdef WINDOWS
    int flag{ SD_RECEIVE };
#else
    int flag{ SHUT_RD };
#endif // WINDOWS

    return (hSocket != NESocket::InvalidSocketHandle ? RETURNED_OK == ::shutdown(hSocket, flag) : false);
}
