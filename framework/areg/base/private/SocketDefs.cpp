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
#include "areg/base/SocketMultiplexer.hpp"
#include "areg/appbase/Application.hpp"

#include "areg/logging/areg_log.h"

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
    #include <netinet/tcp.h>
    #include <netdb.h>
    #include <poll.h>
    #include <sys/socket.h>
    #include <sys/ioctl.h>
    #include <unistd.h>
#endif

#include <regex>
#include <unordered_map>

namespace areg::os {

    // OS specific methods

    /**
     * \brief   OS specific socket initialization. Required in Win32 to initialize resources.
     * \return  Returns true if initialization succeeded.
     **/
    bool _os_init_socket();

    /**
     * \brief   OS specific socket release. Required in Win32 to release resources.
     */
    void _os_release_socket();

    /**
     * \brief   OS specific socket close.
     **/
    void _os_close_socket(SOCKETHANDLE hSocket);

    /**
     * \brief   OS specific send data implementation. All checkups and validations should
     *          be done before calling the method.
     * \return  Returns number of bytes sent via network.
     **/
    int32_t _os_send_data(SOCKETHANDLE hSocket, const uint8_t* dataBuffer, int32_t dataLength);

    /**
     * \brief   Sends messages in windows so that large messages (e.g. 3 MB -> 6 x 512KB windows) never
     *          hand a single oversized buffer to the kernel in one shot.  This matches the
     *          size of thread-cache and keeps per-send latency predictable, requires no extra copy.
     **/
    int32_t _os_send_data_window(SOCKETHANDLE hSocket, const uint8_t* dataBuffer, int32_t dataLength);

    /**
     * \brief   OS specific scatter/gather send. All checkups and validations should
     *          be done before calling the method.
     * \return  Returns total bytes sent; negative on error.
     **/
    int32_t _os_send_data_v(SOCKETHANDLE hSocket, const areg::IoBuffer* buffers, uint32_t count, uint32_t totalSize);

    /**
     * \brief   OS specific receive data implementation. All checkups and validations should
     *          be done before calling the method.
     * \return  Returns number of bytes received via network.
     **/
    int32_t _os_recv_data(SOCKETHANDLE hSocket, uint8_t* dataBuffer, int32_t dataLength);

    /**
     * \brief   Receives exactly \a dataLength bytes from \a hSocket in windowed chunks
     *          of at most DEFAULT_THREAD_CACHE_KB * ONE_KILOBYTE bytes per recv() call.
     *          Loops until all bytes are received; no cache involvement.
     * \return  Returns number of bytes received; negative on error or peer disconnect.
     **/
    int32_t _os_recv_data_window(SOCKETHANDLE hSocket, uint8_t* dataBuffer, int32_t dataLength);

    /**
     * \brief   OS specific implementation of socket control call.
     * \return  Returns true if operation succeeded.
     **/
    bool _os_control(SOCKETHANDLE hSocket, int32_t cmd, unsigned long & arg);

    /**
     * \brief   OS specific implementation of retrieving socket option.
     *          On output the 'value' indicates the value of the option,
     *          which is valid only if function returns true.
     **/
    bool _os_get_option(SOCKETHANDLE hSocket, int32_t level, int32_t name, unsigned long & value);

    /**
     * \brief   OS specific non-blocking connect with timeout.
     *          Sets the socket to non-blocking mode, initiates a connect, then waits
     *          up to timeoutMs milliseconds for the connection to complete.
     *          Restores the socket to blocking mode on success.
     * \param   hSocket     A valid socket handle to connect.
     * \param   addr        Pointer to the target sockaddr structure.
     * \param   addrLen     Size of the sockaddr structure in bytes.
     * \param   timeoutMs   Maximum milliseconds to wait for the connection.
     * \return  Returns true if the connection was established within the timeout.
     **/
    bool _os_connect_socket(SOCKETHANDLE hSocket, const void* addr, uint32_t addrLen, uint32_t timeoutMs);

    /**
     * \brief   Applies platform-specific options that require a connected TCP socket.
     **/
    void _os_configure_connected_socket(SOCKETHANDLE hSocket) noexcept;

} // namespace areg::os

namespace
{
    inline areg::ReceiveMode& _thread_receive_mode(void) noexcept
    {
        static thread_local areg::ReceiveMode _mode{ areg::ReceiveMode::NoCache };
        return _mode;
    }

    inline std::unordered_map<SOCKETHANDLE, areg::ThreadCache>& _thread_local_cache()
    {
        static thread_local std::unordered_map<SOCKETHANDLE, areg::ThreadCache> _rx_caches;
        return _rx_caches;
    }
}


/**
 * \brief   Constant, identifying maximum number of listeners in the queue. Used by server socket when set to listen connection.
 **/
AREG_API_IMPL const int32_t areg::MAXIMUM_LISTEN_QUEUE_SIZE{ SOMAXCONN };

//////////////////////////////////////////////////////////////////////////
// areg::SocketAddress class implementation
//////////////////////////////////////////////////////////////////////////
areg::SocketAddress::SocketAddress()
    : mIpAddr   ( )
    , mHostName ( )
    , mPortNr   ( areg::InvalidPort )
{
}

areg::SocketAddress::SocketAddress(const areg::String& address, uint16_t portNr)
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

bool areg::SocketAddress::to_sockaddr(struct sockaddr_in & sockAddr) const
{
    bool result = false;
    if ( mPortNr != areg::InvalidPort )
    {
        areg::mem_zero(&sockAddr, sizeof(sockAddr));
        sockAddr.sin_family = AF_INET;
        sockAddr.sin_port   = htons( mPortNr );
        if (mIpAddr.is_empty() == false)
        {

#if defined(_MSC_VER) && (_MSC_VER >= 1800)

            result = 1 == ::inet_pton(AF_INET, mIpAddr.as_string(), &sockAddr.sin_addr);

#else   // (_MSC_VER >= 1800) || POSIX

            sockAddr.sin_addr.s_addr = ::inet_addr(mIpAddr.as_string());
            result = true;

#endif  // (_MSC_VER >= 1800) || POSIX

        }
    }

    return result;
}

void areg::SocketAddress::from_sockaddr(const struct sockaddr_in & addrHost)
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
                from_sockaddr(addr_in);
                result = true;
            }
        }
    }

    return result;
}

bool areg::SocketAddress::is_equal(const String& host, uint16_t port) const noexcept
{
    return  (port == mPortNr) &&
            (areg::is_ip_address(host) ? mIpAddr == host : mHostName == host);
}

bool areg::SocketAddress::resolve_address( const String& hostName, uint16_t portNr, bool isServer)
{
    bool result{ false };
    const String& host{ hostName.is_empty() ? areg::LocalHost : hostName };
    mPortNr = areg::InvalidPort;
    mIpAddr.clear();
    mHostName.clear();

    if (areg::is_ip_address(host) == false)
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

        if (areg::RETURNED_OK == ::getaddrinfo(host.as_string(), static_cast<const char*>(svcName), &hints, &aiResult))
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

areg::UserData& areg::UserData::operator = (const areg::UserData& source)
{
    mUser = source.mUser;
    mPassword = source.mPassword;
    return (*this);
}

areg::UserData& areg::UserData::operator = (areg::UserData&& source) noexcept
{
    mUser = std::move(source.mUser);
    mPassword = std::move(source.mPassword);
    return (*this);
}

bool areg::UserData::operator == (const areg::UserData& other) const noexcept
{
    return (mUser == other.mUser) && (mPassword == other.mPassword);
}

bool areg::UserData::operator != (const areg::UserData& other) const noexcept
{
    return (mUser != other.mUser) || (mPassword != other.mPassword);
}

const areg::String& areg::UserData::user() const noexcept
{
    return mUser;
}

void areg::UserData::set_user(const areg::String& user)
{
    mUser = user;
}

const areg::String& areg::UserData::password() const noexcept
{
    return mPassword;
}

void areg::UserData::set_password(const areg::String& password)
{
    mPassword = password;
}

bool areg::UserData::is_valid() const noexcept
{
    return (mUser.is_empty() == false);
}


//////////////////////////////////////////////////////////////////////////
// Socket functions implementation
//////////////////////////////////////////////////////////////////////////
DEF_LOG_SCOPE(areg_base_areg, client_connect);
DEF_LOG_SCOPE(areg_base_areg, server_connect);
DEF_LOG_SCOPE(areg_base_areg, set_recv_size);

AREG_API_IMPL uint32_t areg::thread_cache_size() noexcept
{
    return Application::config_manager().network_cache() * areg::ONE_KILOBYTE;;
}


AREG_API_IMPL SOCKETHANDLE areg::socket_create() noexcept
{
    return static_cast<SOCKETHANDLE>( socket(AF_INET, SOCK_STREAM, IPPROTO_TCP) );
}

AREG_API_IMPL uint32_t areg::max_send_size( SOCKETHANDLE hSocket ) noexcept
{
    ASSERT(is_valid_socket(hSocket));

    unsigned long maxData{ areg::PACKET_DEFAULT_SIZE };
    return (areg::os::_os_get_option(hSocket, SOL_SOCKET, SO_SNDBUF, maxData) ? static_cast<uint32_t>(maxData) : PACKET_DEFAULT_SIZE);
}

AREG_API_IMPL void areg::socket_configure(SOCKETHANDLE hSocket) noexcept
{
    ASSERT(areg::is_valid_socket(hSocket));
    areg::set_send_size(hSocket, areg::SOCKET_SEND_BUFFER_SIZE);
    areg::set_recv_size(hSocket, areg::SOCKET_RECV_BUFFER_SIZE);
}

AREG_API_IMPL void areg::socket_set_no_delay(SOCKETHANDLE hSocket) noexcept
{
    ASSERT(is_valid_socket(hSocket));
    // Disable Nagle algorithm so small RPC messages are sent immediately
    // Only meaningful on connected sockets -- do NOT call on listening sockets.
    constexpr int32_t noDelay{ 1 };
    ::setsockopt(hSocket, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<const char *>(&noDelay), sizeof(noDelay));
    areg::os::_os_configure_connected_socket(hSocket);
}

AREG_API_IMPL uint32_t areg::set_send_size(SOCKETHANDLE hSocket, uint32_t sendSize) noexcept
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

    constexpr uint32_t len{ sizeof(uint32_t) };
    int rc = ::setsockopt(hSocket, SOL_SOCKET, SO_SNDBUF, reinterpret_cast<const char*>(&sendSize), len);

#if defined(__linux__)
    // SO_SNDBUF returns success even when silently clamped to 2×wmem_max -- never use its
    // return code to guard SNDBUFFORCE. Always attempt FORCE unconditionally to bypass the
    // cap; it fails silently without CAP_NET_ADMIN. Not available on macOS or Cygwin.
    ::setsockopt(hSocket, SOL_SOCKET, SO_SNDBUFFORCE, reinterpret_cast<const char*>(&sendSize), len);
#endif  // __linux__

    return (rc == areg::RETURNED_OK ? sendSize : areg::PACKET_MIN_SIZE);
}

AREG_API_IMPL uint32_t areg::max_receive_size( SOCKETHANDLE hSocket ) noexcept
{
    ASSERT(is_valid_socket(hSocket));
    unsigned long maxData{ areg::PACKET_DEFAULT_SIZE };
    return (areg::os::_os_get_option(hSocket, SOL_SOCKET, SO_RCVBUF, maxData) ? static_cast<uint32_t>(maxData) : PACKET_DEFAULT_SIZE);
}

AREG_API_IMPL uint32_t areg::set_recv_size(SOCKETHANDLE hSocket, uint32_t recvSize) noexcept
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

    constexpr uint32_t len{ sizeof(uint32_t) };
    int rc = ::setsockopt(hSocket, SOL_SOCKET, SO_RCVBUF, reinterpret_cast<const char*>(&recvSize), len);

#if defined(__linux__)
    // SO_RCVBUF returns success even when silently clamped to 2×rmem_max -- never use its
    // return code to guard RCVBUFFORCE. Always attempt FORCE unconditionally to bypass the
    // cap; it fails silently without CAP_NET_ADMIN. Not available on macOS or Cygwin.
    ::setsockopt(hSocket, SOL_SOCKET, SO_RCVBUFFORCE, reinterpret_cast<const char*>(&recvSize), len);

    // Warn once if the kernel capped SO_RCVBUF well below what we requested.
    // The kernel reports double the effective window (internal accounting), so divide by 2.
    // Threshold: warn when effective < 25% of requested (severe cap, likely WSL2 default).
    {
        static bool _rcvBufWarned{ false };
        if (!_rcvBufWarned)
        {
            unsigned long actual{ 0u };
            socklen_t actualLen{ static_cast<socklen_t>(sizeof(actual)) };
            if (::getsockopt(hSocket, SOL_SOCKET, SO_RCVBUF,
                             reinterpret_cast<char*>(&actual), &actualLen) == RETURNED_OK)
            {
                const uint32_t effective = static_cast<uint32_t>(actual / 2u);
                if (effective < (recvSize / 4u))
                {
                    _rcvBufWarned = true;
                    LOG_SCOPE(areg_base_areg, set_recv_size);
                    LOG_WARN("SO_RCVBUF capped at %u KB (requested %u KB). "
                             "Raise via: sudo sysctl -w net.core.rmem_max=%u",
                             effective / 1024u, recvSize / 1024u, recvSize);
                }
            }
        }
    }
#endif  // __linux__

    return (rc == areg::RETURNED_OK ? recvSize : areg::PACKET_MIN_SIZE);
}

AREG_API_IMPL SOCKETHANDLE areg::client_connect(const String& hostName, uint16_t portNr, areg::SocketAddress * socketAddr /*= nullptr*/)
{
    LOG_SCOPE( areg_base_areg, client_connect );

    const String host{ hostName.is_empty() ? areg::LocalHost : hostName };

    LOG_DBG("Creating client socket to connect remote host [ %s ] and port number [ %u ]", host.as_string(), static_cast<uint32_t>(portNr));

    if (socketAddr != nullptr)
    {
        socketAddr->reset();
    }

    SOCKETHANDLE result   = areg::InvalidSocketHandle;
    areg::SocketAddress sockAddress;
    if ( sockAddress.resolve_address(host, portNr, false) )
    {
        result = areg::client_connect(sockAddress);
        if ((result != areg::InvalidSocketHandle) && (socketAddr != nullptr))
        {
            *socketAddr = sockAddress;
        }
    }
    else
    {
        LOG_ERR("FAILED to resolve IP address for remote host name [ %s ] and port [ %u ], cannot create client socket", host.as_string(), static_cast<uint32_t>(portNr));
    }

    return result;
}

AREG_API_IMPL SOCKETHANDLE areg::client_connect(const SocketAddress & peerAddr)
{
    LOG_SCOPE( areg_base_areg, client_connect );

    SOCKETHANDLE result   = areg::InvalidSocketHandle;
    if ( peerAddr.is_valid() )
    {
        // struct sockaddr_in remoteAddr = {0};
        sockaddr_in remoteAddr;
        VERIFY( peerAddr.to_sockaddr(remoteAddr) );
        result = areg::socket_create();
        if ( result != areg::InvalidSocketHandle )
        {
            areg::socket_configure(result);

            constexpr uint32_t SOCKET_CONNECT_TIMEOUT_MS { 5'000u };
            if (!areg::os::_os_connect_socket(result, &remoteAddr, sizeof(sockaddr_in), SOCKET_CONNECT_TIMEOUT_MS))
            {
                LOG_ERR("Client failed to connect to remote host [ %s ] and port number [ %u ]. Closing socket [ %u ]"
                            , static_cast<const char *>(peerAddr.host_address())
                            , static_cast<uint32_t>(peerAddr.host_port())
                            , static_cast<uint32_t>(result));

                areg::socket_close(result);
                result = areg::InvalidSocketHandle;
            }
            else
            {
                areg::socket_set_no_delay(result);

#ifdef DEBUG
                LOG_DBG("Client socket [ %u ] succeeded to connect to remote host [ %s ] and port number [ %u ]"
                            , static_cast<uint32_t>(result)
                            , static_cast<const char *>(peerAddr.host_address())
                            , static_cast<uint32_t>(peerAddr.host_port()));
#endif  // DEBUG
            }
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

AREG_API_IMPL SOCKETHANDLE areg::server_connect(const areg::String& hostName, uint16_t portNr, areg::SocketAddress * socketAddr /*= nullptr */)
{
    LOG_SCOPE( areg_base_areg, server_connect );

    String host = hostName.is_empty() ? areg::LocalHost : hostName;

    LOG_DBG("Creating server socket on host [ %s ] and port number [ %u ]", host.as_string(), static_cast<uint32_t>(portNr));


    if (socketAddr != nullptr)
    {
        socketAddr->reset();
    }

    SOCKETHANDLE result   = areg::InvalidSocketHandle;
    areg::SocketAddress sockAddress;
    if ( sockAddress.resolve_address(host, portNr, true) )
    {
        result = areg::server_connect(sockAddress);
        if ((result != areg::InvalidSocketHandle) && (socketAddr != nullptr))
        {
            areg::socket_set_no_delay(result);
            *socketAddr = sockAddress;
        }
    }
    else
    {
        LOG_ERR("FAILED to resolve IP address for host name [ %s ] and port [ %u ], cannot create server socket", host.as_string(), static_cast<uint32_t>(portNr));
    }

    return result;
}

AREG_API_IMPL SOCKETHANDLE areg::server_connect(const areg::SocketAddress & peerAddr)
{
    LOG_SCOPE( areg_base_areg, server_connect );

    SOCKETHANDLE result   = areg::InvalidSocketHandle;
    if ( peerAddr.is_valid() )
    {
        // struct sockaddr_in remoteAddr = {0};
        sockaddr_in serverAddr;
        VERIFY( peerAddr.to_sockaddr(serverAddr) );
        result = areg::socket_create();
        if ( result != areg::InvalidSocketHandle )
        {
            areg::socket_configure(result);

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

AREG_API_IMPL bool areg::server_listen(SOCKETHANDLE serverSocket, int32_t maxQueueSize /*= areg::MAXIMUM_LISTEN_QUEUE_SIZE*/) noexcept
{
    return ( (serverSocket != areg::InvalidSocketHandle) && (areg::RETURNED_OK == ::listen(serverSocket, maxQueueSize)) );
}


DEBUG_DEF_LOG_SCOPE(areg_base_areg, server_accept);
AREG_API_IMPL SOCKETHANDLE areg::server_accept(areg::SocketMultiplexer & multiplexer, SOCKETHANDLE serverSocket, areg::SocketAddress * socketAddr /*= nullptr*/, int32_t timeoutMs /*= static_cast<int32_t>(areg::WAIT_INFINITE)*/)
{
    DEBUG_LOG_SCOPE( areg_base_areg, server_accept );
    DEBUG_LOG_DBG("Checking server socket event (persistent mux), server socket handle [ %u ]", static_cast<uint32_t>(serverSocket));

    if (serverSocket == areg::InvalidSocketHandle)
    {
        DEBUG_LOG_WARN("Invalid server socket, cannot accept connection");
        return areg::InvalidSocketHandle;
    }

    if (socketAddr != nullptr)
    {
        socketAddr->reset();
    }

    const SOCKETHANDLE readable = multiplexer.wait(timeoutMs);
    if (readable == serverSocket)
    {
        // New client connection pending.
        struct sockaddr_in acceptAddr;
        areg::mem_zero(&acceptAddr, sizeof(sockaddr_in));
        socklen_t len = sizeof(sockaddr_in);

        DEBUG_LOG_DBG("... server waiting for new connection event ...");
        SOCKETHANDLE result = ::accept(serverSocket, reinterpret_cast<sockaddr *>(&acceptAddr), &len);
        DEBUG_LOG_DBG("Server accepted new connection of client socket [ %u ]", static_cast<uint32_t>(result));

        if (result != areg::InvalidSocketHandle)
        {
            areg::socket_configure(result);
            areg::socket_set_no_delay(result);
            if (socketAddr != nullptr)
            {
                socketAddr->from_sockaddr(acceptAddr);
            }
        }

        return result;
    }
    else if (readable != areg::InvalidSocketHandle && readable != areg::FailedSocketHandle)
    {
        DEBUG_LOG_DBG("Server selected event of existing client socket [ %u ] connection", static_cast<uint32_t>(readable));
        return readable;
    }
    else if (readable == areg::InvalidSocketHandle)
    {
        return areg::InvalidSocketHandle;
    }
    else
    {
        DEBUG_LOG_ERR("Failed to wait for connection. The server socket [ %u ] might be closed and not valid anymore", static_cast<uint32_t>(serverSocket));
        return areg::FailedSocketHandle;
    }
}

AREG_API_IMPL SOCKETHANDLE areg::server_accept(SOCKETHANDLE serverSocket, const SOCKETHANDLE * masterList, int32_t entriesCount, areg::SocketAddress * socketAddr /*= nullptr*/)
{
    DEBUG_LOG_SCOPE( areg_base_areg, server_accept );
    DEBUG_LOG_DBG("Checking server socket event, server socket handle [ %u ]", static_cast<uint32_t>(serverSocket));

    SOCKETHANDLE result = areg::InvalidSocketHandle;
    if (masterList == nullptr)
    {
        DEBUG_LOG_ERR("Invalid list of sockets, cannot accept connection");
        return result;
    }

    if (socketAddr != nullptr)
    {
        socketAddr->reset();
    }

    if ( serverSocket != areg::InvalidSocketHandle )
    {
        DEBUG_LOG_DBG("Waiting for socket event on server socket [ %u ] with [ %d ] client(s)", static_cast<uint32_t>(serverSocket), entriesCount);

        const SOCKETHANDLE readable = areg::SocketMultiplexer{}.wait(serverSocket, masterList, entriesCount);
        if (readable == serverSocket)
        {
            // New client connection pending.
            struct sockaddr_in acceptAddr;
            areg::mem_zero(&acceptAddr, sizeof(sockaddr_in));
            socklen_t len = sizeof(sockaddr_in);

            DEBUG_LOG_DBG("... server waiting for new connection event ...");
            result = ::accept(serverSocket, reinterpret_cast<sockaddr *>(&acceptAddr), &len);
            DEBUG_LOG_DBG("Server accepted new connection of client socket [ %u ]", static_cast<uint32_t>(result));

            if (result != areg::InvalidSocketHandle)
            {
                areg::socket_configure(result);
                areg::socket_set_no_delay(result);
                if (socketAddr != nullptr)
                {
                    socketAddr->from_sockaddr(acceptAddr);
                }
            }
        }
        else if (readable != areg::InvalidSocketHandle && readable != areg::FailedSocketHandle)
        {
            // Existing client has data (or connection closed).
            result = readable;
            DEBUG_LOG_DBG("Server selected event of existing client socket [ %u ] connection", static_cast<uint32_t>(result));
        }
        else
        {
            DEBUG_LOG_ERR("Failed to wait for connection. The server socket [ %u ] might be closed and not valid anymore", static_cast<uint32_t>(serverSocket));
            result = areg::FailedSocketHandle;
        }
    }
    else
    {
        DEBUG_LOG_WARN("Found broken connection of socket [ %u ]", static_cast<size_t>(result));
        return result;
    }

    return result;
}

AREG_API_IMPL bool areg::is_socket_alive(SOCKETHANDLE hSocket) noexcept
{
    unsigned long error = 0;
    return (areg::is_valid_socket(hSocket) && areg::os::_os_get_option(hSocket, SOL_SOCKET, SO_ERROR, error) && (error == 0));
}

AREG_API_IMPL uint32_t areg::pending_read(SOCKETHANDLE hSocket) noexcept
{
    unsigned long result = 0;
    return (areg::is_valid_socket(hSocket) && areg::os::_os_control(hSocket, FIONREAD, result) ? static_cast<uint32_t>(result) : 0);
}

AREG_API_IMPL bool areg::socket_initialize() noexcept
{
    return areg::os::_os_init_socket();
}

AREG_API_IMPL void areg::socket_release() noexcept
{
    areg::os::_os_release_socket();
}

AREG_API_IMPL void areg::socket_close(SOCKETHANDLE hSocket) noexcept
{
    if (areg::is_valid_socket(hSocket))
    {
        areg::os::_os_close_socket(hSocket);
    }
}

AREG_API_IMPL bool areg::set_send_timeout(SOCKETHANDLE hSocket, uint32_t timeoutMs) noexcept
{
    if (!areg::is_valid_socket(hSocket))
        return false;

#ifdef _WIN32
    DWORD timeout = static_cast<DWORD>(timeoutMs);
    return ::setsockopt(static_cast<SOCKET>(hSocket), SOL_SOCKET, SO_SNDTIMEO
                        , reinterpret_cast<const char *>(&timeout), sizeof(timeout)) == 0;
#else
    struct timeval tv;
    tv.tv_sec  = static_cast<decltype(tv.tv_sec)>(timeoutMs / 1000u);
    tv.tv_usec = static_cast<decltype(tv.tv_usec)>((timeoutMs % 1000u) * 1000u);
    return ::setsockopt(static_cast<int>(hSocket), SOL_SOCKET, SO_SNDTIMEO
                        , reinterpret_cast<const char *>(&tv), sizeof(tv)) == 0;
#endif
}

AREG_API_IMPL int32_t areg::send_data(SOCKETHANDLE hSocket, const uint8_t* dataBuffer, uint32_t dataLength) noexcept
{
    int32_t result = -1;
    if (areg::is_valid_socket(hSocket))
    {
        result = 0;
        if ((dataBuffer != nullptr) && (static_cast<int32_t>(dataLength) != 0))
        {
            result = areg::os::_os_send_data(hSocket, dataBuffer, static_cast<int32_t>(dataLength));
        }
    }

    return result;
}

AREG_API_IMPL int32_t areg::send_data_v(SOCKETHANDLE hSocket, const areg::IoBuffer* buffers, uint32_t count, uint32_t totalSize) noexcept
{
    if (!areg::is_valid_socket(hSocket))
        return -1;

    if ((buffers == nullptr) || (count == 0u))
        return 0;

    return areg::os::_os_send_data_v(hSocket, buffers, count, totalSize);
}

AREG_API_IMPL int32_t areg::receive_data(SOCKETHANDLE hSocket, uint8_t* dataBuffer, uint32_t dataLength) noexcept
{
    if (!areg::is_valid_socket(hSocket))
        return -1;

    if ((dataBuffer == nullptr) || (dataLength == 0u))
        return 0;

    return areg::os::_os_recv_data(hSocket, dataBuffer, static_cast<int32_t>(dataLength));
}

AREG_API_IMPL int32_t areg::receive_data_window(SOCKETHANDLE hSocket, uint8_t* dataBuffer, uint32_t dataLength) noexcept
{
    if (!areg::is_valid_socket(hSocket))
        return -1;

    if ((dataBuffer == nullptr) || (dataLength == 0u))
        return 0;

    return areg::os::_os_recv_data_window(hSocket, dataBuffer, static_cast<int32_t>(dataLength));
}

AREG_API_IMPL uint32_t areg::recv_data_available(SOCKETHANDLE hSocket) noexcept
{
    if (areg::receive_mode() == areg::ReceiveMode::NoCache)
        return 0u;

    const areg::ThreadCache& tc = areg::thread_rx_cache(hSocket);
    if (tc.unread == 0u)
        return 0u;

    if (tc.unread < static_cast<uint32_t>(sizeof(areg::MessageHeader)))
        return 0u;

    const areg::MessageHeader* hdr{ reinterpret_cast<const areg::MessageHeader*>(tc.buffer.get() + tc.head) };
    ASSERT(hdr != nullptr);
    const uint32_t msg_total = static_cast<uint32_t>(sizeof(areg::MessageHeader)) + hdr->rbhBufHeader.biUsed;
    return (tc.unread >= msg_total) ? tc.unread : 0u;
}

AREG_API_IMPL bool areg::disable_send(SOCKETHANDLE hSocket) noexcept
{
#ifdef _WIN32
    int32_t flag{ SD_SEND };
#else
    int32_t flag{ SHUT_WR };
#endif // _WIN32

    return (areg::is_valid_socket(hSocket) && (areg::RETURNED_OK == ::shutdown(hSocket, flag)) );
}

AREG_API_IMPL bool areg::disable_receive(SOCKETHANDLE hSocket) noexcept
{
#ifdef _WIN32
    int32_t flag{ SD_RECEIVE };
#else
    int32_t flag{ SHUT_RD };
#endif // _WIN32

    return (areg::is_valid_socket(hSocket) && (areg::RETURNED_OK == ::shutdown(hSocket, flag)) );
}

AREG_API_IMPL void areg::socket_interrupt(SOCKETHANDLE hSocket) noexcept
{
    if (areg::is_valid_socket(hSocket))
    {
#ifdef _WIN32
        ::shutdown(static_cast<SOCKET>(hSocket), SD_BOTH);
#else
        ::shutdown(static_cast<int>(hSocket), SHUT_RDWR);
#endif
    }
}

AREG_API_IMPL const areg::String & areg::hostname() noexcept
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

AREG_API_IMPL bool areg::is_ip_address(const areg::String& ipaddress) noexcept
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
        char c{ ip[static_cast<size_t>(i)] };

        if (c == '.')
        {
            if ((++dots > 3) || (i == 0) || (ip[static_cast<size_t>(i - 1)] == '.'))
                return false;

            if (num < 0 || num > 255)
                return false;

            num = 0;
            continue;
        }

        if (areg::is_numeric(c) == false)
            return false;

        num = num * 10 + (c - '0');
        if (num > 255)
            return false;
    }

    return (dots == 3) && (num >= 0) && (num <= 255) && (ip.back() != '.');

#else

    // 25[0-5]  --> 250-255
    // 2[0-4]\d --> 200-249
    // 1\d{2}   --> 100-199
    // [1-9]?\d --> 0-99
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

AREG_API_IMPL uint16_t areg::extract_port_number(const sockaddr_in& addrHost) noexcept
{
    return ntohs(addrHost.sin_port);
}

AREG_API_IMPL areg::ThreadCache& areg::thread_tx_cache() noexcept
{
    static thread_local areg::ThreadCache _tx_cache;
    return _tx_cache;
}

AREG_API_IMPL areg::ThreadCache& areg::thread_rx_cache(SOCKETHANDLE hSocket) noexcept
{
    if (receive_mode() == ReceiveMode::MonoCache)
    {
        static thread_local areg::ThreadCache _rx_cache;
        _rx_cache.socket = hSocket;
        return _rx_cache;
    }
    else
    {
        std::unordered_map<SOCKETHANDLE, areg::ThreadCache>& map = _thread_local_cache();
        areg::ThreadCache& tc = map[hSocket];
        tc.socket = hSocket;
        return tc;
    }
}

AREG_API_IMPL void areg::thread_rx_cache_release(SOCKETHANDLE hSocket) noexcept
{
    std::unordered_map<SOCKETHANDLE, areg::ThreadCache>& map = _thread_local_cache();
    auto found = map.find(hSocket);
    if (found != map.end())
        map.erase(hSocket);
}

AREG_API_IMPL void areg::set_receive_mode(ReceiveMode mode) noexcept
{
    _thread_receive_mode() = mode;
}

AREG_API_IMPL areg::ReceiveMode areg::receive_mode() noexcept
{
    return _thread_receive_mode();
}
