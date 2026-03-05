/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/SocketDefs.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform. Socket OS independent methods
 ************************************************************************/
#include "areg/base/SocketDefs.hpp"

#include "areg/base/MemoryDefs.hpp"
#include "areg/logging/areg_log.h"

#include <algorithm>

#ifdef   _WIN32
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif  // WIN32_LEAN_AND_MEAN
    #ifndef NOMINMAX
        #define NOMINMAX
    #endif  // NOMINMAX
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
#include <regex>

namespace areg::os {

    // OS specific methods

    /**
     * \brief   OS specific socket initialization. Required in Win32 to initialize resources.
     * \return  Returns true if initialization succeeded.
     **/
    bool _osInitSocket();

    /**
     * \brief   OS specific socket release. Required in Win32 to release resources.
     */
    void _osReleaseSocket();

    /**
     * \brief   OS specific socket close.
     */
    void _osCloseSocket(SOCKETHANDLE hSocket);

    /**
     * \brief   OS specific send data implementation. All checkups and validations should
     *          be done before calling the method.
     * \return  Returns number of bytes sent via network.
     */
    int32_t _osSendData(SOCKETHANDLE hSocket, const uint8_t* dataBuffer, int32_t dataLength, int32_t blockMaxSize);

    /**
     * \brief   OS specific receive data implementation. All checkups and validations should
     *          be done before calling the method.
     * \return  Returns number of bytes received via network.
     */
    int32_t _osRecvData(SOCKETHANDLE hSocket, uint8_t* dataBuffer, int32_t dataLength, int32_t blockMaxSize);

    /**
     * \brief   OS specific implementation of socket control call.
     * \return  Returns true if operation succeeded.
     */
    bool _osControl(SOCKETHANDLE hSocket, int32_t cmd, unsigned long & arg);

    /**
     * \brief   OS specific implementation of retrieving socket option.
     *          On output the 'value' indicates the value of the option,
     *          which is valid only if function returns true.
     */
    bool _osGetOption(SOCKETHANDLE hSocket, int32_t level, int32_t name, unsigned long & value);
} // namespace areg::os


namespace areg {

/**
 * \brief   Constant, identifying maximum number of listeners in the queue. Used by server socket when set to listen connection.
 **/
AREG_API_IMPL const int32_t MAXIMUM_LISTEN_QUEUE_SIZE{ SOMAXCONN };

//////////////////////////////////////////////////////////////////////////
// areg::SocketAddress class implementation
//////////////////////////////////////////////////////////////////////////
areg::SocketAddress::SocketAddress()
    : mIpAddr   ( )
    , mHostName ( )
    , mPortNr   ( areg::InvalidPort )
{
}

areg::SocketAddress::SocketAddress(const String& address, uint16_t portNr)
    : mIpAddr   ( )
    , mHostName ( )
    , mPortNr   ( portNr )
{
    if (areg::is_ip_address(address))
    {
        mIpAddr     = address;
        mHostName   = areg::ip_to_host(address);
    }
    else
    {
        mIpAddr     = areg::host_to_ip(address);
        mHostName   = address;
    }
}

areg::SocketAddress::SocketAddress(const areg::SocketAddress & src)
    : mIpAddr   ( src.mIpAddr )
    , mHostName ( src.mHostName )
    , mPortNr   ( src.mPortNr )
{
}

areg::SocketAddress::SocketAddress( areg::SocketAddress && src ) noexcept
    : mIpAddr   ( std::move(src.mIpAddr) )
    , mHostName ( std::move(src.mHostName) )
    , mPortNr   ( std::move(src.mPortNr) )
{
}

areg::SocketAddress & areg::SocketAddress::operator = ( const areg::SocketAddress & src )
{
    mIpAddr     = src.mIpAddr;
    mHostName   = src.mHostName;
    mPortNr     = src.mPortNr;

    return (*this);
}

areg::SocketAddress & areg::SocketAddress::operator = ( areg::SocketAddress && src ) noexcept
{
    mIpAddr     = std::move(src.mIpAddr);
    mHostName   = std::move(src.mHostName);
    mPortNr     = std::move(src.mPortNr);

    return (*this);
}

bool areg::SocketAddress::address(struct sockaddr_in & out_sockAddr) const
{
    bool result = false;
    if ( mPortNr != areg::InvalidPort )
    {
        areg::mem_zero(&out_sockAddr, sizeof(out_sockAddr));
        out_sockAddr.sin_family = AF_INET;
        out_sockAddr.sin_port   = htons( mPortNr );
        if (mIpAddr.is_empty() == false)
        {

#if defined(_MSC_VER) && (_MSC_VER >= 1800)

            result = 1 == ::inet_pton(AF_INET, mIpAddr.as_string(), &out_sockAddr.sin_addr);

#else   // (_MSC_VER >= 1800) || POSIX

            out_sockAddr.sin_addr.s_addr = ::inet_addr(mIpAddr.as_string());
            result = true;

#endif  // (_MSC_VER >= 1800) || POSIX

        }
    }

    return result;
}

void areg::SocketAddress::set_address(const struct sockaddr_in & addrHost)
{
    mPortNr     = areg::extract_port_number(addrHost);
    mIpAddr     = areg::extract_ip_address(addrHost);
    mHostName   = areg::ip_to_host(mIpAddr);
}

bool areg::SocketAddress::resolve_socket(SOCKETHANDLE hSocket)
{
    bool result{ false };
    mPortNr     = areg::InvalidPort;
    mIpAddr.clear();
    mHostName.clear();

    if ( hSocket != areg::InvalidSocketHandle )
    {
        struct sockaddr_in sAddr;
        areg::mem_zero(&sAddr, sizeof(sockaddr));

        socklen_t len = sizeof(sockaddr);
        if (areg::RETURNED_OK == ::getpeername(hSocket, reinterpret_cast<struct sockaddr *>(&sAddr), &len) )
        {
            sockaddr_in & addr_in = reinterpret_cast<sockaddr_in &>(sAddr);
            if ( addr_in.sin_family == AF_INET )
            {
                set_address(addr_in);
                result = true;
            }
        }
    }

    return result;
}

bool areg::SocketAddress::is_equal_address(const String& host, uint16_t port) const
{
    return  (port == mPortNr) &&
            (areg::is_ip_address(host) ? mIpAddr == host : mHostName == host);
}

bool areg::SocketAddress::resolve_address( const std::string_view & hostName, uint16_t portNr, bool isServer)
{
    bool result{ false };
    const std::string_view& host{ hostName.empty() ? areg::LocalHost : hostName };
    mPortNr = areg::InvalidPort;
    mIpAddr.clear();
    mHostName.clear();

    if (areg::is_ip_address(String(host)) == false)
    {
        // acquire address info
        char svcName[0x0F];
        String::format_string(svcName, 0x0F, "%u", portNr);

        struct addrinfo hints;
        areg::mem_zero(&hints, sizeof(addrinfo));
        hints.ai_family     = AF_INET;
        hints.ai_socktype   = SOCK_STREAM;
        hints.ai_flags      = isServer ? AI_PASSIVE : 0;
        hints.ai_protocol   = IPPROTO_TCP;
        addrinfo * aiResult = nullptr;

        if (areg::RETURNED_OK == ::getaddrinfo(host.data(), static_cast<const char*>(svcName), &hints, &aiResult))
        {
            ASSERT(aiResult != nullptr);
            for ( addrinfo * addrInfo = aiResult; addrInfo != nullptr; addrInfo = addrInfo->ai_next)
            {
                if ( addrInfo->ai_family == AF_INET && addrInfo->ai_socktype == SOCK_STREAM )
                {
                    struct sockaddr_in * addrIn = reinterpret_cast<struct sockaddr_in *>(addrInfo->ai_addr);
                    mIpAddr     = areg::extract_ip_address(*addrIn);
                    mHostName   = host;
                    mPortNr     = portNr;
                    result      = mIpAddr.is_empty() == false;
                    break;
                }
            }

            ::freeaddrinfo( aiResult );
        }
    }
    else
    {
        mPortNr     = portNr;
        mIpAddr     = host;
        mHostName   = areg::ip_to_host(mIpAddr);
        result      = true;
    }

    return result;
}

bool areg::SocketAddress::operator == ( const areg::SocketAddress & other ) const
{
    return (this != &other ? mIpAddr == other.mIpAddr && mPortNr == other.mPortNr : true);
}

bool areg::SocketAddress::operator != ( const areg::SocketAddress & other ) const
{
    return (this != &other ? mIpAddr != other.mIpAddr || mPortNr != other.mPortNr : false);
}

//////////////////////////////////////////////////////////////////////////
// areg::UserData class implementation
//////////////////////////////////////////////////////////////////////////

areg::UserData::UserData()
    : mUser     ( )
    , mPassword ( )
{
}

areg::UserData::UserData(const String& user, const String& password)
    : mUser     ( user )
    , mPassword ( password )
{
}

areg::UserData::UserData(const areg::UserData& src)
    : mUser     ( src.mUser )
    , mPassword ( src.mPassword )
{
}

areg::UserData::UserData(areg::UserData&& src) noexcept
    : mUser     ( std::move(src.mUser) )
    , mPassword ( std::move(src.mPassword) )
{
}

areg::UserData& areg::UserData::operator=(const areg::UserData& source)
{
    mUser = source.mUser;
    mPassword = source.mPassword;
    return (*this);
}

areg::UserData& areg::UserData::operator=(areg::UserData&& source) noexcept
{
    mUser = std::move(source.mUser);
    mPassword = std::move(source.mPassword);
    return (*this);
}

bool areg::UserData::operator==(const areg::UserData& other)
{
    return (mUser == other.mUser) && (mPassword == other.mPassword);
}

bool areg::UserData::operator!=(const areg::UserData& other)
{
    return (mUser != other.mUser) || (mPassword != other.mPassword);
}

const String& areg::UserData::user() const noexcept
{
    return mUser;
}

void areg::UserData::set_user(const String& user)
{
    mUser = user;
}

const String& areg::UserData::password() const noexcept
{
    return mPassword;
}

void areg::UserData::set_password(const String& password)
{
    mPassword = password;
}

bool areg::UserData::is_valid() const noexcept
{
    return (mUser.is_empty() == false);
}

} // namespace areg

//////////////////////////////////////////////////////////////////////////
// Socket functions implementation
//////////////////////////////////////////////////////////////////////////
DEF_LOG_SCOPE(areg_base_areg_client_connect);
DEF_LOG_SCOPE(areg_base_areg_server_connect);
DEF_LOG_SCOPE(areg_base_areg_server_accept);

AREG_API_IMPL SOCKETHANDLE areg::socket_create()
{
    return static_cast<SOCKETHANDLE>( socket(AF_INET, SOCK_STREAM, IPPROTO_TCP) );
}

AREG_API_IMPL uint32_t areg::max_send_size( SOCKETHANDLE hSocket )
{
    ASSERT(is_valid_socket(hSocket));

    unsigned long maxData{ areg::PACKET_DEFAULT_SIZE };
    return (areg::os::_osGetOption(hSocket, SOL_SOCKET, SO_SNDBUF, maxData) ? static_cast<uint32_t>(maxData) : PACKET_DEFAULT_SIZE);
}

AREG_API_IMPL uint32_t areg::set_send_size(SOCKETHANDLE hSocket, uint32_t sendSize)
{
    ASSERT(is_valid_socket(hSocket));

    if (sendSize == 0)
    {
        sendSize = areg::PACKET_DEFAULT_SIZE;
    }
    else if (sendSize < areg::PACKET_MIN_SIZE)
    {
        sendSize = areg::PACKET_MIN_SIZE;
    }
    else if (sendSize > areg::PACKET_MAX_SIZE)
    {
        sendSize = areg::PACKET_MAX_SIZE;
    }

    constexpr uint32_t len{ sizeof(uint32_t) };
    return (areg::RETURNED_OK == ::setsockopt(hSocket, SOL_SOCKET, SO_SNDBUF, reinterpret_cast<const char*>(&sendSize), len) ? sendSize : areg::PACKET_MIN_SIZE);
}

AREG_API_IMPL uint32_t areg::max_receive_size( SOCKETHANDLE hSocket )
{
    ASSERT(is_valid_socket(hSocket));
    unsigned long maxData{ areg::PACKET_DEFAULT_SIZE };
    return (areg::os::_osGetOption(hSocket, SOL_SOCKET, SO_RCVBUF, maxData) ? static_cast<uint32_t>(maxData) : PACKET_DEFAULT_SIZE);
}

AREG_API_IMPL uint32_t areg::set_recv_size(SOCKETHANDLE hSocket, uint32_t recvSize)
{
    ASSERT(is_valid_socket(hSocket));

    if (recvSize == 0)
    {
        recvSize = areg::PACKET_DEFAULT_SIZE;
    }
    else if (recvSize < areg::PACKET_MIN_SIZE)
    {
        recvSize = areg::PACKET_MIN_SIZE;
    }
    else if (recvSize > areg::PACKET_MAX_SIZE)
    {
        recvSize = areg::PACKET_MAX_SIZE;
    }

    constexpr uint32_t len{ sizeof(uint32_t) };
    return (areg::RETURNED_OK == ::setsockopt(hSocket, SOL_SOCKET, SO_RCVBUF, reinterpret_cast<const char*>(&recvSize), len) ? recvSize : areg::PACKET_MIN_SIZE);
}

AREG_API_IMPL SOCKETHANDLE areg::client_connect(const std::string_view & hostName, uint16_t portNr, areg::SocketAddress * out_socketAddr /*= nullptr*/)
{
    LOG_SCOPE(areg_base_areg_client_connect);

    const char * host = hostName.empty() ? areg::LocalHost.data() : hostName.data();

    LOG_DBG("Creating client socket to connect remote host [ %s ] and port number [ %u ]", host, static_cast<uint32_t>(portNr));

    if (out_socketAddr != nullptr)
    {
        out_socketAddr->reset();
    }

    SOCKETHANDLE result   = areg::InvalidSocketHandle;
    areg::SocketAddress sockAddress;
    if ( sockAddress.resolve_address(host, portNr, false) )
    {
        result = areg::client_connect(sockAddress);
        if ((result != areg::InvalidSocketHandle) && (out_socketAddr != nullptr))
        {
            *out_socketAddr = sockAddress;
        }
    }
    else
    {
        LOG_ERR("FAILED to resolve IP address for remote host name [ %s ] and port [ %u ], cannot create client socket", host, static_cast<uint32_t>(portNr));
    }

    return result;
}

AREG_API_IMPL SOCKETHANDLE areg::client_connect(const SocketAddress & peerAddr)
{
    LOG_SCOPE(areg_base_areg_client_connect);

    SOCKETHANDLE result   = areg::InvalidSocketHandle;
    if ( peerAddr.is_valid() )
    {
        // struct sockaddr_in remoteAddr = {0};
        sockaddr_in remoteAddr;
        VERIFY( peerAddr.address(remoteAddr) );
        result = areg::socket_create();
        if ( result != areg::InvalidSocketHandle )
        {
            if (areg::RETURNED_OK != ::connect(result, reinterpret_cast<sockaddr *>(&remoteAddr), sizeof(sockaddr_in)))
            {
                LOG_ERR("Client failed to connect to remote host [ %s ] and port number [ %u ]. Closing socket [ %u ]"
                            , static_cast<const char *>(peerAddr.host_address())
                            , static_cast<uint32_t>(peerAddr.host_port())
                            , static_cast<uint32_t>(result));

                areg::socket_close(result);
                result = areg::InvalidSocketHandle;
            }
#ifdef DEBUG
            else
            {
                LOG_DBG("Client socket [ %u ] succeeded to connect to remote host [ %s ] and port number [ %u ]"
                            , static_cast<uint32_t>(result)
                            , static_cast<const char *>(peerAddr.host_address())
                            , static_cast<uint32_t>(peerAddr.host_port()));
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
        LOG_ERR("Address [ %s ] or port number [ %u ] is not valid. No client is created", static_cast<const char *>(peerAddr.host_address()), peerAddr.host_port());
    }

    return result;
}

AREG_API_IMPL SOCKETHANDLE areg::server_connect(const std::string_view & hostName, uint16_t portNr, SocketAddress * out_socketAddr /*= nullptr */)
{
    LOG_SCOPE(areg_base_areg_server_connect);

    const char * host = hostName.empty() ? areg::LocalHost.data() : hostName.data();

    LOG_DBG("Creating server socket on host [ %s ] and port number [ %u ]", host, static_cast<uint32_t>(portNr));


    if (out_socketAddr != nullptr)
    {
        out_socketAddr->reset();
    }

    SOCKETHANDLE result   = areg::InvalidSocketHandle;
    areg::SocketAddress sockAddress;
    if ( sockAddress.resolve_address(host, portNr, true) )
    {
        result = areg::server_connect(sockAddress);
        if ( result != areg::InvalidSocketHandle && out_socketAddr != nullptr )
            *out_socketAddr = sockAddress;
    }
    else
    {
        LOG_ERR("FAILED to resolve IP address for host name [ %s ] and port [ %u ], cannot create server socket", host, static_cast<uint32_t>(portNr));
    }

    return result;
}

AREG_API_IMPL SOCKETHANDLE areg::server_connect(const SocketAddress & peerAddr)
{
    LOG_SCOPE(areg_base_areg_server_connect);

    SOCKETHANDLE result   = areg::InvalidSocketHandle;
    if ( peerAddr.is_valid() )
    {
        // struct sockaddr_in remoteAddr = {0};
        sockaddr_in serverAddr;
        VERIFY( peerAddr.address(serverAddr) );
        result = areg::socket_create();
        if ( result != areg::InvalidSocketHandle )
        {
            int32_t yes = 1;    // avoid the "address already in use" error message
            ::setsockopt( result, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char *>(&yes), sizeof(int32_t) );
            if (areg::RETURNED_OK != bind(result, reinterpret_cast<sockaddr *>(&serverAddr), sizeof(sockaddr_in)) )
            {
                LOG_ERR("Server failed to bind on host [ %s ] and port number [ %u ]. Closing socket [ %u ]"
                            , static_cast<const char *>(peerAddr.host_address())
                            , static_cast<uint32_t>(peerAddr.host_port())
                            , static_cast<uint32_t>(result));

                areg::socket_close( result );
                result = areg::InvalidSocketHandle;
            }
#ifdef  DEBUG
            else
            {
                LOG_DBG("Server socket [ %u ] succeeded to bind on host [ %s ] and port number [ %u ]. Ready to listen."
                          , static_cast<uint32_t>(result)
                          , static_cast<const char *>(peerAddr.host_address())
                          , static_cast<uint32_t>(peerAddr.host_port()));

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
        LOG_ERR("Address [ %s ] or port number [ %u ] is not valid. No server is created", static_cast<const char *>(peerAddr.host_address()), peerAddr.host_port());
    }

    return result;
}

AREG_API_IMPL bool areg::server_listen(SOCKETHANDLE serverSocket, int32_t maxQueueSize /*= areg::MAXIMUM_LISTEN_QUEUE_SIZE*/)
{
    return ( (serverSocket != areg::InvalidSocketHandle) && (areg::RETURNED_OK == ::listen(serverSocket, maxQueueSize)) );
}

AREG_API_IMPL SOCKETHANDLE areg::server_accept(SOCKETHANDLE serverSocket, const SOCKETHANDLE * masterList, int32_t entriesCount, areg::SocketAddress * out_socketAddr /*= nullptr*/)
{
    LOG_SCOPE(areg_base_areg_server_accept);
    LOG_DBG("Checking server socket event, server socket handle [ %u ]", static_cast<uint32_t>(serverSocket));

    SOCKETHANDLE result = areg::InvalidSocketHandle;
    if (masterList == nullptr)
    {
        LOG_ERR("Invalid list of sockets, cannot accept connection");
        return result;
    }

    if (out_socketAddr != nullptr)
    {
        out_socketAddr->reset();
    }

    if ( serverSocket != areg::InvalidSocketHandle )
    {
        fd_set readList { };
        FD_ZERO(&readList);
        FD_SET( serverSocket, &readList );
        SOCKETHANDLE maxSocket = serverSocket;

        if ( entriesCount > 0 )
        {
            entriesCount= std::min(entriesCount, static_cast<int32_t>((FD_SETSIZE - 1)));

#ifdef  _WIN32

            for ( int count = 0; count < entriesCount; ++ count)
                readList.fd_array[count + 1] = masterList[count];
            
            readList.fd_count = static_cast<u_int>( entriesCount + 1 );

#else   // !_WIN32

            LOG_DBG("There are [ %d ] socket entries in the master list, setting FD_SET", entriesCount);
            for ( int count = 0; count < entriesCount; ++ count)
            {
                SOCKETHANDLE sh = masterList[count];
                if (areg::is_socket_alive(sh))
                {
                    FD_SET(masterList[count], &readList);
                    maxSocket = std::max(maxSocket, sh);
                }
                else
                {
                    LOG_WARN("Found socket [ %u ] that is not alive anymore, break accepting connection to handle error.", ((uint32_t)sh));
                    result = sh;
                    break;
                }
            }

#endif  // !_WIN32
        }

        if (result == areg::InvalidSocketHandle)
        {
            LOG_DBG("Call select to wait socket connection, max socket value is [ %d ]", static_cast<int32_t>(maxSocket));
            int32_t selected    = select( static_cast<int32_t>(maxSocket) + 1 /* param is ignored in Win32*/, &readList, nullptr, nullptr, nullptr);
            if ( selected > 0 )
            {
                if ( FD_ISSET(serverSocket, &readList) != 0 )
                {
                    // have got new client connection. resolve and get socket
                    struct sockaddr_in acceptAddr; // connecting client address information
                    areg::mem_zero(&acceptAddr, sizeof(sockaddr_in));

                    socklen_t len = sizeof(sockaddr_in);
                    LOG_DBG("... server waiting for new connection event ...");
                    result = ::accept( serverSocket, reinterpret_cast<sockaddr *>(&acceptAddr), &len );
                    LOG_DBG("Server accepted new connection of client socket [ %u ]", static_cast<uint32_t>(result));
                    if ((result != areg::InvalidSocketHandle) && (out_socketAddr != nullptr))
                    {
                        out_socketAddr->set_address(acceptAddr);
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
                            LOG_DBG("Server selected event of existing client socket [ %u ] connection", static_cast<uint32_t>(result));
                            break;
                        }
                    }
                }
            }
            else
            {
                LOG_ERR("Failed to select connection. The server socket [ %u ] might be closed and not valid anymore, return value [ %d ]", static_cast<uint32_t>(serverSocket), selected);
                result = areg::FailedSocketHandle;
            }
        }
        else
        {
            LOG_ERR("Invalid server socket, ignoring accept connections!");
        }
    }
    else
    {
        LOG_WARN("Found broken connection of socket [ %u ]", static_cast<size_t>(result));
        return result;
    }

    return result;
}

AREG_API_IMPL bool areg::is_socket_alive(SOCKETHANDLE hSocket)
{
    unsigned long error = 0;
    return (areg::is_valid_socket(hSocket) && areg::os::_osGetOption(hSocket, SOL_SOCKET, SO_ERROR, error) && (error == 0));
}

AREG_API_IMPL uint32_t areg::pending_read(SOCKETHANDLE hSocket)
{
    unsigned long result = 0;
    return (areg::is_valid_socket(hSocket) && areg::os::_osControl(hSocket, FIONREAD, result) ? static_cast<uint32_t>(result) : 0);
}

AREG_API_IMPL bool areg::socket_initialize()
{
    return areg::os::_osInitSocket();
}

AREG_API_IMPL void areg::socket_release()
{
    areg::os::_osReleaseSocket();
}

AREG_API_IMPL void areg::socket_close(SOCKETHANDLE hSocket)
{
    if (areg::is_valid_socket(hSocket))
    {
        areg::os::_osCloseSocket(hSocket);
    }
}

AREG_API_IMPL int32_t areg::send_data(SOCKETHANDLE hSocket, const uint8_t* dataBuffer, uint32_t dataLength, uint32_t blockMaxSize /*= areg::DEFAULT_SIZE*/)
{
    int32_t result = -1;
    if (areg::is_valid_socket(hSocket))
    {
        result = 0;
        if ((dataBuffer != nullptr) && (static_cast<int32_t>(dataLength) > 0))
        {
            result = areg::os::_osSendData(hSocket, dataBuffer, static_cast<int32_t>(dataLength), static_cast<int32_t>(blockMaxSize) != 0 ? static_cast<int32_t>(blockMaxSize) : static_cast<int32_t>(max_send_size(hSocket)));
        }
    }

    return result;
}

AREG_API_IMPL int32_t areg::receive_data(SOCKETHANDLE hSocket, uint8_t* dataBuffer, uint32_t dataLength, uint32_t blockMaxSize )
{
    int32_t result = -1;

    if (areg::is_valid_socket(hSocket))
    {
        result = 0;
        if ((dataBuffer != nullptr) && (static_cast<int32_t>(dataLength) > 0))
        {
            result = areg::os::_osRecvData(hSocket, dataBuffer, static_cast<int32_t>(dataLength), static_cast<int32_t>(blockMaxSize) > 0 ? static_cast<int32_t>(blockMaxSize) : static_cast<int32_t>(max_receive_size(hSocket)));
        }
    }

    return result;
}

AREG_API_IMPL bool areg::disable_send(SOCKETHANDLE hSocket)
{
#ifdef _WIN32
    int32_t flag{ SD_SEND };
#else
    int32_t flag{ SHUT_WR };
#endif // _WIN32

    return (areg::is_valid_socket(hSocket) && (areg::RETURNED_OK == ::shutdown(hSocket, flag)) );
}

AREG_API_IMPL bool areg::disable_receive(SOCKETHANDLE hSocket)
{
#ifdef _WIN32
    int32_t flag{ SD_RECEIVE };
#else
    int32_t flag{ SHUT_RD };
#endif // _WIN32

    return (areg::is_valid_socket(hSocket) && (areg::RETURNED_OK == ::shutdown(hSocket, flag)) );
}

AREG_API_IMPL const areg::String & areg::hostname()
{
    static String result;

    if ( result.is_empty( ) )
    {
        // if not initialized
        constexpr size_t length{ 256 };
        char name[ length ]{};
        if ( ::gethostname( name, length ) == areg::RETURNED_OK )
        {
            result = name;
        }
    }

    return result;
}

AREG_API_IMPL bool areg::is_ip_address(const areg::String& ipaddress)
{
#if 1   // use without exception

    const std::string& ip = ipaddress.data();
    int32_t num{ 0 };
    int32_t dots{ 0 };
    int32_t len = static_cast<int32_t>(ip.length());

    if (len < 7 || len > 15)  // Fast length check (e.g., "0.0.0.0" to "255.255.255.255")
        return false;

    for (int i = 0; i < len; ++i)
    {
        char c{ ip[i] };

        if (c == '.')
        {
            if ((++dots > 3) || (i == 0) || (ip[i - 1] == '.'))
                return false;

            if (num < 0 || num > 255)
                return false;

            num = 0;
            continue;
        }

        if (std::isdigit(static_cast<uint8_t>(c)) == false)
            return false;

        num = num * 10 + (c - '0');
        if (num > 255)
            return false;
    }

    return (dots == 3) && (num >= 0) && (num <= 255) && (ip.back() != '.');

#else

    // 25[0-5]  --> 250�255
    // 2[0-4]\d --> 200�249
    // 1\d{2}   --> 100�199
    // [1-9]?\d --> 0�99
    static const std::regex ipv4Regex(
        R"(^(25[0-5]|2[0-4][0-9]|1[0-9]{2}|[1-9]?[0-9])\.)"
        R"(25[0-5]|2[0-4][0-9]|1[0-9]{2}|[1-9]?[0-9])\.)"
        R"(25[0-5]|2[0-4][0-9]|1[0-9]{2}|[1-9]?[0-9])\.)"
        R"(25[0-5]|2[0-4][0-9]|1[0-9]{2}|[1-9]?[0-9])$)",
        std::regex::ECMAScript | std::regex::optimize
    );

    try
    {
        return std::regex_match(ipaddress.data(), ipv4Regex);
    }
    catch (const std::regex_error&)
    {
        return false; // Regex failed or input caused an issue
    }

#endif
}

AREG_API_IMPL areg::String areg::host_to_ip(const areg::String& hostName)
{
    String ipAddress(hostName);

    addrinfo hints{}, * result = nullptr;
    hints.ai_family = AF_INET; // IPv4 only
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    if ((::getaddrinfo(hostName.as_string(), nullptr, &hints, &result) == 0) && (result != nullptr))
    {
        sockaddr_in* addr = reinterpret_cast<sockaddr_in*>(result->ai_addr);
        char buffer[INET_ADDRSTRLEN]{};
        if (::inet_ntop(AF_INET, &addr->sin_addr, buffer, sizeof(buffer)) != nullptr)
        {
            ipAddress = buffer;
        }

        ::freeaddrinfo(result);
    }

    return ipAddress;
}

AREG_API_IMPL areg::String areg::ip_to_host(const areg::String& ipAddress)
{
    String hostName(ipAddress);

    sockaddr_in sa{};
    sa.sin_family = AF_INET;
    if (::inet_pton(AF_INET, ipAddress.as_string(), &sa.sin_addr) == 1)
    {
        char host[NI_MAXHOST]{};
        if (::getnameinfo(reinterpret_cast<sockaddr*>(&sa), sizeof(sa), host, sizeof(host), nullptr, 0, 0) == 0)
        {
            hostName = host;
        }
    }

    return hostName;
}

AREG_API_IMPL areg::String areg::extract_ip_address(const sockaddr_in& addrHost)
{
    String result;
#if defined(_MSC_VER) && (_MSC_VER >= 1800)

    char ipAddr[64] { };
    IN_ADDR& inAddr = const_cast<IN_ADDR&>(addrHost.sin_addr);
    if (nullptr != ::inet_ntop(AF_INET, &inAddr, ipAddr, 64))
    {
        result = ipAddr;
    }

#else   // (_MSC_VER >= 1800) || POSIX

    result = ::inet_ntoa(addrHost.sin_addr);

#endif  // (_MSC_VER >= 1800) || POSIX

    return result;
}

AREG_API_IMPL uint16_t areg::extract_port_number(const sockaddr_in& addrHost)
{
    return ntohs(addrHost.sin_port);
}
