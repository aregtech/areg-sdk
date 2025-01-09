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
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform. Socket OS independent methods
 ************************************************************************/
#include "areg/base/NESocket.hpp"

#include "areg/base/GEMacros.h"
#include "areg/base/NEMemory.hpp"
#include "areg/logging/GELog.h"

#ifdef   _WINDOWS
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif  // WIN32_LEAN_AND_MEAN
    #include <WinSock2.h>
    #include <WS2tcpip.h>
#else
    #include <arpa/inet.h>
    #include <ctype.h>      // IEEE Std 1003.1-2001
    #include <netinet/in.h>
    #include <netdb.h>
    #include <sys/socket.h>
    #include <sys/ioctl.h>
    #include <sys/select.h>
    #include <unistd.h>
#endif

#include <utility>

DEF_LOG_SCOPE(areg_base_NESocket_clientSocketConnect);
DEF_LOG_SCOPE(areg_base_NESocket_serverSocketConnect);
DEF_LOG_SCOPE(areg_base_NESocket_serverAcceptConnection);

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

NESocket::SocketAddress::SocketAddress(const String& address, uint16_t portNr)
    : mIpAddr   ( address )
    , mPortNr   ( portNr )
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
            }
        }
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
                    break;
                }
            }

            ::freeaddrinfo( aiResult );
        }
    }
    else
    {
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
// NESocket::UserData class implementation
//////////////////////////////////////////////////////////////////////////

NESocket::UserData::UserData(void)
    : mUser     ( )
    , mPassword ( )
{
}

NESocket::UserData::UserData(const String& user, const String& password)
    : mUser     ( user )
    , mPassword ( password )
{
}

NESocket::UserData::UserData(const UserData& src)
    : mUser     ( src.mUser )
    , mPassword ( src.mPassword )
{
}

NESocket::UserData::UserData(UserData&& src) noexcept
    : mUser     ( std::move(src.mUser) )
    , mPassword ( std::move(src.mPassword) )
{
}

NESocket::UserData& NESocket::UserData::operator=(const NESocket::UserData& source)
{
    mUser = source.mUser;
    mPassword = source.mPassword;
    return (*this);
}

NESocket::UserData& NESocket::UserData::operator=(NESocket::UserData&& source) noexcept
{
    mUser = std::move(source.mUser);
    mPassword = std::move(source.mPassword);
    return (*this);
}

bool NESocket::UserData::operator==(const NESocket::UserData& other)
{
    return (mUser == other.mUser) && (mPassword == other.mPassword);
}

bool NESocket::UserData::operator!=(const NESocket::UserData& other)
{
    return (mUser != other.mUser) || (mPassword != other.mPassword);
}

const String& NESocket::UserData::getUser(void) const
{
    return mUser;
}

void NESocket::UserData::setUser(const String& user)
{
    mUser = user;
}

const String& NESocket::UserData::getPassword(void) const
{
    return mPassword;
}

void NESocket::UserData::setPassword(const String& password)
{
    mPassword = password;
}

bool NESocket::UserData::isValid(void) const
{
    return (mUser.isEmpty() == false);
}

//////////////////////////////////////////////////////////////////////////
// NESocket namespace functions implementation
//////////////////////////////////////////////////////////////////////////

AREG_API_IMPL SOCKETHANDLE NESocket::socketCreate( void )
{
    return static_cast<SOCKETHANDLE>( socket(AF_INET, SOCK_STREAM, IPPROTO_TCP) );
}

AREG_API_IMPL unsigned int NESocket::getMaxSendSize( SOCKETHANDLE hSocket )
{
    ASSERT(isSocketHandleValid(hSocket));

    unsigned long maxData{ NESocket::PACKET_DEFAULT_SIZE };
    return (_osGetOption(hSocket, SOL_SOCKET, SO_SNDBUF, maxData) ? static_cast<unsigned int>(maxData) : NESocket::PACKET_DEFAULT_SIZE);
}

AREG_API_IMPL unsigned int NESocket::setMaxSendSize(SOCKETHANDLE hSocket, unsigned int sendSize)
{
    ASSERT(isSocketHandleValid(hSocket));

    if (sendSize == 0)
    {
        sendSize = NESocket::PACKET_DEFAULT_SIZE;
    }
    else if (sendSize < NESocket::PACKET_MIN_SIZE)
    {
        sendSize = NESocket::PACKET_MIN_SIZE;
    }
    else if (sendSize > NESocket::PACKET_MAX_SIZE)
    {
        sendSize = NESocket::PACKET_MAX_SIZE;
    }

    constexpr unsigned int len{ sizeof(unsigned int) };
    return (RETURNED_OK == ::setsockopt(hSocket, SOL_SOCKET, SO_SNDBUF, reinterpret_cast<const char*>(&sendSize), len) ? sendSize : NESocket::PACKET_MIN_SIZE);
}

AREG_API_IMPL unsigned int NESocket::getMaxReceiveSize( SOCKETHANDLE hSocket )
{
    ASSERT(isSocketHandleValid(hSocket));
    unsigned long maxData{ NESocket::PACKET_DEFAULT_SIZE };
    return (_osGetOption(hSocket, SOL_SOCKET, SO_RCVBUF, maxData) ? static_cast<unsigned int>(maxData) : NESocket::PACKET_DEFAULT_SIZE);
}

AREG_API_IMPL unsigned int NESocket::setMaxReceiveSize(SOCKETHANDLE hSocket, unsigned int recvSize)
{
    ASSERT(isSocketHandleValid(hSocket));

    if (recvSize == 0)
    {
        recvSize = NESocket::PACKET_DEFAULT_SIZE;
    }
    else if (recvSize < NESocket::PACKET_MIN_SIZE)
    {
        recvSize = NESocket::PACKET_MIN_SIZE;
    }
    else if (recvSize > NESocket::PACKET_MAX_SIZE)
    {
        recvSize = NESocket::PACKET_MAX_SIZE;
    }

    constexpr unsigned int len{ sizeof(unsigned int) };
    return (RETURNED_OK == ::setsockopt(hSocket, SOL_SOCKET, SO_RCVBUF, reinterpret_cast<const char*>(&recvSize), len) ? recvSize : NESocket::PACKET_MIN_SIZE);
}

AREG_API_IMPL SOCKETHANDLE NESocket::clientSocketConnect(const std::string_view & hostName, unsigned short portNr, NESocket::SocketAddress * out_socketAddr /*= nullptr*/)
{
    LOG_SCOPE(areg_base_NESocket_clientSocketConnect);

    const char * host = hostName.empty() ? NESocket::LocalHost.data() : hostName.data();

    LOG_DBG("Creating client socket to connect remote host [ %s ] and port number [ %u ]", host, static_cast<unsigned int>(portNr));

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
        LOG_ERR("FAILED to resolve IP address for remote host name [ %s ] and port [ %u ], cannot create client socket", host, static_cast<unsigned int>(portNr));
    }

    return result;
}

AREG_API_IMPL SOCKETHANDLE NESocket::clientSocketConnect(const SocketAddress & peerAddr)
{
    LOG_SCOPE(areg_base_NESocket_clientSocketConnect);

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
                LOG_ERR("Client failed to connect to remote host [ %s ] and port number [ %u ]. Closing socket [ %u ]"
                            , static_cast<const char *>(peerAddr.getHostAddress())
                            , static_cast<unsigned int>(peerAddr.getHostPort())
                            , static_cast<unsigned int>(result));

                NESocket::socketClose(result);
                result = NESocket::InvalidSocketHandle;
            }
#ifdef DEBUG
            else
            {
                LOG_DBG("Client socket [ %u ] succeeded to connect to remote host [ %s ] and port number [ %u ]"
                            , static_cast<unsigned int>(result)
                            , static_cast<const char *>(peerAddr.getHostAddress())
                            , static_cast<unsigned int>(peerAddr.getHostPort()));
            }
#endif  // DEBUG
        }
        else
        {
            LOG_ERR("Failed to create socket, cannot create client!");
        }
    }
    else
    {
        LOG_ERR("Address [ %s ] or port number [ %u ] is not valid. No client is created", static_cast<const char *>(peerAddr.getHostAddress()), peerAddr.getHostPort());
    }

    return result;
}

AREG_API_IMPL SOCKETHANDLE NESocket::serverSocketConnect(const std::string_view & hostName, unsigned short portNr, SocketAddress * out_socketAddr /*= nullptr */)
{
    LOG_SCOPE(areg_base_NESocket_serverSocketConnect);

    const char * host = hostName.empty() ? NESocket::LocalHost.data() : hostName.data();

    LOG_DBG("Creating server socket on host [ %s ] and port number [ %u ]", host, static_cast<unsigned int>(portNr));


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
        LOG_ERR("FAILED to resolve IP address for host name [ %s ] and port [ %u ], cannot create server socket", host, static_cast<unsigned int>(portNr));
    }

    return result;
}

AREG_API_IMPL SOCKETHANDLE NESocket::serverSocketConnect(const SocketAddress & peerAddr)
{
    LOG_SCOPE(areg_base_NESocket_serverSocketConnect);

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
                LOG_ERR("Server failed to bind on host [ %s ] and port number [ %u ]. Closing socket [ %u ]"
                            , static_cast<const char *>(peerAddr.getHostAddress())
                            , static_cast<unsigned int>(peerAddr.getHostPort())
                            , static_cast<unsigned int>(result));

                NESocket::socketClose( result );
                result = NESocket::InvalidSocketHandle;
            }
#ifdef  DEBUG
            else
            {
                LOG_DBG("Server socket [ %u ] succeeded to bind on host [ %s ] and port number [ %u ]. Ready to listen."
                          , static_cast<unsigned int>(result)
                          , static_cast<const char *>(peerAddr.getHostAddress())
                          , static_cast<unsigned int>(peerAddr.getHostPort()));

            }
#endif  // DEBUG
        }
        else
        {
            LOG_ERR("Failed to create socket, cannot create server!");
        }
    }
    else
    {
        LOG_ERR("Address [ %s ] or port number [ %u ] is not valid. No server is created", static_cast<const char *>(peerAddr.getHostAddress()), peerAddr.getHostPort());
    }

    return result;
}

AREG_API_IMPL bool NESocket::serverListenConnection(SOCKETHANDLE serverSocket, int maxQueueSize /*= NESocket::MAXIMUM_LISTEN_QUEUE_SIZE*/)
{
    return ( (serverSocket != NESocket::InvalidSocketHandle) && (RETURNED_OK == listen(serverSocket, maxQueueSize)) );
}

AREG_API_IMPL SOCKETHANDLE NESocket::serverAcceptConnection(SOCKETHANDLE serverSocket, const SOCKETHANDLE * masterList, int entriesCount, NESocket::SocketAddress * out_socketAddr /*= nullptr*/)
{
    LOG_SCOPE(areg_base_NESocket_serverAcceptConnection);
    LOG_DBG("Checking server socket event, server socket handle [ %u ]", static_cast<unsigned int>(serverSocket));

    SOCKETHANDLE result = NESocket::InvalidSocketHandle;
    if (masterList == nullptr)
    {
        LOG_ERR("Invalid list of sockets, cannot accept connection");
        return result;
    }

    if (out_socketAddr != nullptr)
    {
        out_socketAddr->resetAddress();
    }

    if ( serverSocket != NESocket::InvalidSocketHandle )
    {
        fd_set readList { };
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

            LOG_DBG("There are [ %d ] socket entries in the master list, setting FD_SET", entriesCount);
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
                    LOG_WARN("Found socket [ %u ] that is not alive anymore, break accepting connection to handle error.", ((unsigned int)sh));
                    result = sh;
                    break;
                }
            }

#endif  // !_WINDOWS
        }

        if (result == NESocket::InvalidSocketHandle)
        {
            LOG_DBG("Call select to wait socket connection, max socket value is [ %d ]", static_cast<int>(maxSocket));
            int selected    = select( static_cast<int>(maxSocket) + 1 /* param is ignored in Win32*/, &readList, nullptr, nullptr, nullptr);
            if ( selected > 0 )
            {
                if ( FD_ISSET(serverSocket, &readList) != 0 )
                {
                    // have got new client connection. resolve and get socket
                    struct sockaddr_in acceptAddr; // connecting client address information
                    NEMemory::memZero(&acceptAddr, sizeof(sockaddr_in));

                    socklen_t len = sizeof(sockaddr_in);
                    LOG_DBG("... server waiting for new connection event ...");
                    result = ::accept( serverSocket, reinterpret_cast<sockaddr *>(&acceptAddr), &len );
                    LOG_DBG("Server accepted new connection of client socket [ %u ]", static_cast<unsigned int>(result));
                    if ((result != NESocket::InvalidSocketHandle) && (out_socketAddr != nullptr))
                    {
                        out_socketAddr->setAddress(acceptAddr);
                    }
                }
                else
                {
                    LOG_DBG("Have got select event of existing connection, going to resolve socket");

                    //  check whether have got connection from existing clients. if 'yes', server can read data.
                    for ( int count = 0; count < entriesCount; ++ count )
                    {
                        if (FD_ISSET( masterList[count], &readList ) != 0)
                        {
                            result = masterList[count];
                            LOG_DBG("Server selected event of existing client socket [ %u ] connection", static_cast<unsigned int>(result));
                            break;
                        }
                    }
                }
            }
            else
            {
                LOG_ERR("Failed to select connection. The server socket [ %u ] might be closed and not valid anymore, return value [ %d ]", static_cast<unsigned int>(serverSocket), selected);
                result = NESocket::FailedSocketHandle;
            }
        }
        else
        {
            LOG_ERR("Invalid server socket, ignoring accept connections!");
        }
    }
    else
    {
        LOG_WARN("Found broken connection of socket [ %u ]", MACRO_MAKE_NUMBER(result));
        return result;
    }

    return result;
}

AREG_API_IMPL bool NESocket::isSocketAlive(SOCKETHANDLE hSocket)
{
    unsigned long error = 0;
    return (isSocketHandleValid(hSocket) && _osGetOption(hSocket, SOL_SOCKET, SO_ERROR, error) && (error == 0));
}

AREG_API_IMPL unsigned int NESocket::pendingRead(SOCKETHANDLE hSocket)
{
    unsigned long result = 0;
    return (isSocketHandleValid(hSocket) && _osControl(hSocket, FIONREAD, result) ? static_cast<unsigned int>(result) : 0);
}

AREG_API_IMPL void NESocket::socketRelease(void)
{
    _osReleaseSocket();
}

AREG_API_IMPL void NESocket::socketClose(SOCKETHANDLE hSocket)
{
    if (isSocketHandleValid(hSocket))
    {
        _osCloseSocket(hSocket);
    }
}

AREG_API_IMPL int NESocket::sendData(SOCKETHANDLE hSocket, const unsigned char* dataBuffer, uint32_t dataLength, uint32_t blockMaxSize /*= NECommon::DEFAULT_SIZE*/)
{
    int result = -1;
    if (isSocketHandleValid(hSocket))
    {
        result = 0;
        if ((dataBuffer != nullptr) && (static_cast<int32_t>(dataLength) > 0))
        {
            result = _osSendData(hSocket, dataBuffer, static_cast<int32_t>(dataLength), static_cast<int32_t>(blockMaxSize) != 0 ? static_cast<int32_t>(blockMaxSize) : static_cast<int32_t>(NESocket::getMaxSendSize(hSocket)));
        }
    }

    return result;
}

AREG_API_IMPL int NESocket::receiveData(SOCKETHANDLE hSocket, unsigned char* dataBuffer, uint32_t dataLength, uint32_t blockMaxSize )
{
    int result = -1;

    if (isSocketHandleValid(hSocket))
    {
        result = 0;
        if ((dataBuffer != nullptr) && (static_cast<int32_t>(dataLength) > 0))
        {
            result = _osRecvData(hSocket, dataBuffer, static_cast<int32_t>(dataLength), static_cast<int32_t>(blockMaxSize) > 0 ? static_cast<int32_t>(blockMaxSize) : static_cast<int32_t>(NESocket::getMaxReceiveSize(hSocket)));
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

    return ( isSocketHandleValid(hSocket) && (RETURNED_OK == ::shutdown(hSocket, flag)) );
}

AREG_API_IMPL bool NESocket::disableReceive(SOCKETHANDLE hSocket)
{
#ifdef WINDOWS
    int flag{ SD_RECEIVE };
#else
    int flag{ SHUT_RD };
#endif // WINDOWS

    return ( isSocketHandleValid(hSocket) && (RETURNED_OK == ::shutdown(hSocket, flag)) );
}

AREG_API_IMPL const String & NESocket::getHostname(void)
{
    static String result;

    if ( result.isEmpty( ) )
    {
        // if not initialized
        constexpr size_t length{ 256 };
        char name[ length ]{};
        if ( gethostname( name, length ) == RETURNED_OK )
        {
            result = name;
        }
    }

    return result;
}
