#ifndef AREG_BASE_SOCKETDEFS_HPP
#define AREG_BASE_SOCKETDEFS_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/SocketDefs.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, OS-independent socket wrapper: address management,
 *              connection helpers, and data transfer utilities for TCP/IP (IPv4).
 *
 * \note        Currently only TCP/IP IPv4 connections are supported.
 *              All other connection types are out of scope.
 *
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/String.hpp"

#include <string_view>
#include "areg/base/SocketMultiplexer.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
struct sockaddr_in;

namespace areg {

//////////////////////////////////////////////////////////////////////////
// areg::SocketAddress
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Stores an IPv4 address and port number for a socket endpoint.
 *          Resolves host names to IP addresses and extracts peer addresses
 *          from connected sockets.
 **/
class AREG_API SocketAddress
{
//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    SocketAddress();

    SocketAddress(const String& address, uint16_t portNr);

    SocketAddress(const SocketAddress& source);

    SocketAddress(SocketAddress&& source) noexcept;

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    SocketAddress& operator = (const SocketAddress& source);

    SocketAddress& operator = (SocketAddress&& source) noexcept;

    /**
     * \brief   Returns true if both address and port are identical.
     **/
    [[nodiscard]] bool operator == (const SocketAddress& other) const;

    /**
     * \brief   Returns true if either address or port differs.
     **/
    [[nodiscard]] bool operator != (const SocketAddress& other) const;

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Fills \a sockAddr with the IPv4 address and port stored in
     *          this object.
     *
     * \param   sockAddr    Output sockaddr_in structure to populate.
     * \return  true on success; false if the stored address is invalid.
     **/
    bool to_sockaddr(struct sockaddr_in& sockAddr) const;

    /**
     * \brief   Sets the IP address and port from a sockaddr_in structure.
     *
     * \param   sockAddr    Source sockaddr_in structure.
     **/
    void from_sockaddr(const struct sockaddr_in& sockAddr);

    /**
     * \brief   Resolves \a hostName to an IPv4 address and stores it together
     *          with \a portNr.  If \a hostName is nullptr or empty, the
     *          loopback address is used for server sockets; the wildcard
     *          address is used for client sockets.
     *
     * \param   hostName    Numeric IP or human-readable host name to resolve.
     * \param   portNr      Port number to store.
     * \param   isServer    Pass true when binding a server socket, false for
     *                      a client connection.
     * \return  true if resolution succeeded; false otherwise.
     **/
    bool resolve_address(const String& hostName, uint16_t portNr, bool isServer);

    /**
     * \brief   Reads the peer address of the connected socket \a hSocket
     *          and stores the result in this object.
     *
     * \param   hSocket     Descriptor of an already-connected socket.
     * \return  true if the address was retrieved successfully; false otherwise.
     **/
    bool resolve_socket(SOCKETHANDLE hSocket);

    /**
     * \brief   Returns true if the stored address equals \a host and \a port.
     **/
    [[nodiscard]]
    bool is_equal(const String& host, uint16_t port) const;

    /**
     * \brief   Returns the numeric IPv4 address string (e.g. "192.168.1.1").
     **/
    [[nodiscard]] inline const String& host_address() const noexcept;

    /**
     * \brief   Returns the resolved host name string.
     **/
    [[nodiscard]] inline const String& host_name() const noexcept;

    /**
     * \brief   Returns the port number.
     **/
    [[nodiscard]] inline uint16_t host_port() const noexcept;

    /**
     * \brief   Clears the stored address and resets the port to InvalidPort.
     **/
    inline void reset() noexcept;

    /**
     * \brief   Returns true if the IP address is non-empty and the port is
     *          not InvalidPort.
     **/
    [[nodiscard]] inline bool is_valid() const noexcept;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    String   mIpAddr;   //!< Numeric IPv4 address string.
    String   mHostName; //!< Resolved host name string.
    uint16_t mPortNr;   //!< Port number; InvalidPort when not set.
};

//////////////////////////////////////////////////////////////////////////
// areg::UserData
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Stores a user name and password pair for authenticated connections.
 **/
class AREG_API UserData
{
//////////////////////////////////////////////////////////////////////////
// Constructors
//////////////////////////////////////////////////////////////////////////
public:
    UserData();

    UserData(const String& user, const String& password);

    UserData(const UserData& src);

    UserData(UserData&& src) noexcept;

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    UserData& operator = (const UserData& source);

    UserData& operator = (UserData&& source) noexcept;

    /**
     * \brief   Returns true if both user name and password are identical.
     **/
    // BUG FIX: was missing 'const' — comparison must not mutate the object.
    [[nodiscard]] bool operator == (const UserData& other) const noexcept;

    /**
     * \brief   Returns true if user name or password differs.
     **/
    // BUG FIX: was missing 'const' — comparison must not mutate the object.
    [[nodiscard]] bool operator != (const UserData& other) const noexcept;

    /**
     * \brief   Returns the user name.
     **/
    [[nodiscard]] const String& user() const noexcept;

    /**
     * \brief   Sets the user name.
     **/
    void set_user(const String& user);

    /**
     * \brief   Returns the password.
     **/
    [[nodiscard]] const String& password() const noexcept;

    /**
     * \brief   Sets the password.
     **/
    void set_password(const String& password);

    /**
     * \brief   Returns true if the user name is not empty.
     **/
    [[nodiscard]] bool is_valid() const noexcept;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    String mUser;       //!< User name.
    String mPassword;   //!< User password.
};

//////////////////////////////////////////////////////////////////////////
// areg namespace constants
//////////////////////////////////////////////////////////////////////////

/// Invalid socket descriptor.
constexpr SOCKETHANDLE  InvalidSocketHandle     { static_cast<SOCKETHANDLE>(~0) };

/// Sentinel returned by server-accept failures (distinct from InvalidSocketHandle).
constexpr SOCKETHANDLE  FailedSocketHandle      { static_cast<SOCKETHANDLE>(~1) };

/// Sentinel for an unset or invalid port number.
constexpr uint16_t      InvalidPort             { 0 };

/// Loopback host name.
constexpr std::string_view LocalHost            { "localhost" };

/// Loopback IPv4 address.
constexpr std::string_view LocalAddress         { "127.0.0.1" };

/// Separator character between IPv4 octets.
constexpr char          IP_SEPARATOR            { '.' };

/// Buffer size required to hold any IPv4 address string (e.g. "255.255.255.255\0").
constexpr uint32_t      IP_ADDRESS_SIZE         { 16 };

/// Minimum payload size in bytes for a single send or receive operation.
constexpr uint32_t      PACKET_MIN_SIZE         { 512 };

/// Default payload size in bytes for a single send or receive operation.
constexpr uint32_t      PACKET_DEFAULT_SIZE     { 1500 };

/// Maximum payload size in bytes for a single send or receive operation.
constexpr uint32_t      PACKET_MAX_SIZE         { 65536 };

/// Sentinel indicating an invalid or uninitialized packet size.
constexpr uint32_t      PACKET_INVALID_SIZE     { 0 };

/// Kernel send-buffer size (SO_SNDBUF) applied to every new socket.
/// 4 MB allows full-throughput pipelining without stalling on ACKs.
constexpr uint32_t      SOCKET_SEND_BUFFER_SIZE { 4u * 1024u * 1024u };

/// Kernel receive-buffer size (SO_RCVBUF) applied to every new socket.
/// 4 MB matches the send side and prevents receiver-side head-of-line blocking.
constexpr uint32_t      SOCKET_RECV_BUFFER_SIZE { 4u * 1024u * 1024u };

/**
 * \brief   Maximum number of pending connections the OS will queue on a
 *          listening server socket (SOMAXCONN).  Initialized at runtime.
 **/
extern AREG_API const int32_t MAXIMUM_LISTEN_QUEUE_SIZE /*= SOMAXCONN*/;

//////////////////////////////////////////////////////////////////////////
// areg namespace free functions
//////////////////////////////////////////////////////////////////////////

// areg::SocketMultiplexer is defined in areg/base/SocketMultiplexer.hpp
// (included above).

//////////////////////////////////////////////////////////////////////////
// areg namespace free functions
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Returns true if \a hSocket is a usable descriptor (not
 *          InvalidSocketHandle and not FailedSocketHandle).
 **/
[[nodiscard]] inline bool is_valid_socket(SOCKETHANDLE hSocket) noexcept;

/**
 * \brief   Initializes the socket subsystem for the current process.
 *          Subsequent calls are reference-counted until socket_release()
 *          is called an equal number of times.
 *
 * \return  true on success; false otherwise.
 **/
AREG_API bool socket_initialize();

/**
 * \brief   Decrements the socket subsystem reference count; releases
 *          resources when it reaches zero.
 **/
AREG_API void socket_release();

/**
 * \brief   Creates a streaming TCP/IP socket.
 *
 * \return  Valid descriptor on success; InvalidSocketHandle on failure.
 **/
AREG_API SOCKETHANDLE socket_create();

/**
 * \brief   Closes \a hSocket.  No further I/O is possible afterward.
 **/
AREG_API void socket_close(SOCKETHANDLE hSocket);

/**
 * \brief   Applies recommended socket options to \a hSocket immediately
 *          after creation or acceptance.  Sets SO_SNDBUF to
 *          SOCKET_SEND_BUFFER_SIZE and SO_RCVBUF to SOCKET_RECV_BUFFER_SIZE.
 *
 * \param   hSocket     Valid socket descriptor to configure.
 **/
AREG_API void socket_configure(SOCKETHANDLE hSocket);

/**
 * \brief   Disables the Nagle algorithm (TCP_NODELAY) on a connected socket.
 *          Call this only on client or accepted sockets, never on listening sockets.
 *
 * \param   hSocket     Valid connected socket descriptor.
 **/
AREG_API void socket_set_no_delay(SOCKETHANDLE hSocket);

/**
 * \brief   Creates a TCP/IP client socket and connects to \a peerAddr.
 *
 * \return  Valid descriptor on success; InvalidSocketHandle on failure.
 **/
AREG_API SOCKETHANDLE client_connect(const SocketAddress& peerAddr);

/**
 * \brief   Creates a TCP/IP client socket and connects to \a hostName:\a portNr.
 *
 * \param   hostName    Host name or numeric IPv4 address of the remote server.
 * \param   portNr      Remote port number.
 * \param   socketAddr  If not nullptr, receives the resolved remote address.
 * \return  Valid descriptor on success; InvalidSocketHandle on failure.
 **/
AREG_API SOCKETHANDLE client_connect(const String& hostName, uint16_t portNr, SocketAddress* socketAddr = nullptr);

/**
 * \brief   Creates a TCP/IP server socket and binds to \a peerAddr.
 *          Call server_listen() before accepting connections.
 *
 * \return  Valid descriptor on success; InvalidSocketHandle on failure.
 **/
AREG_API SOCKETHANDLE server_connect(const SocketAddress& peerAddr);

/**
 * \brief   Creates a TCP/IP server socket and binds to \a hostName:\a portNr.
 *          Call server_listen() before accepting connections.
 *
 * \param   hostName    Host name or numeric IPv4 address to bind to.
 * \param   portNr      Port number to bind to.
 * \param   socketAddr  If not nullptr, receives the bound local address.
 * \return  Valid descriptor on success; InvalidSocketHandle on failure.
 **/
AREG_API SOCKETHANDLE server_connect(const String& hostName, uint16_t portNr, SocketAddress* socketAddr = nullptr);

/**
 * \brief   Places \a serverSocket into listening mode.
 *
 * \param   serverSocket    Bound server socket descriptor.
 * \param   maxQueueSize    Maximum number of pending connections the OS will
 *                          queue (default: MAXIMUM_LISTEN_QUEUE_SIZE).
 * \return  true if the socket is now listening; false otherwise.
 **/
AREG_API bool server_listen(SOCKETHANDLE serverSocket, int32_t maxQueueSize = areg::MAXIMUM_LISTEN_QUEUE_SIZE);

/**
 * \brief   Waits via the persistent \a multiplexer, then accepts a pending
 *          client connection if the server socket fired.
 *
 *          All sockets must have been registered with \a multiplexer before
 *          calling this overload.  On Linux this uses epoll_wait() instead
 *          of poll(), giving O(1) readiness notification.
 *
 * \param   multiplexer     Persistent multiplexer with the server and client
 *                          sockets already registered.
 * \param   serverSocket    The listening server socket descriptor.
 * \param   socketAddr      If not nullptr, receives the new client's address
 *                          when a new connection is accepted.
 * \return  Valid descriptor for the new connection or an existing readable
 *          client; FailedSocketHandle if \a serverSocket is invalid;
 *          InvalidSocketHandle on timeout or other failure.
 **/
AREG_API SOCKETHANDLE server_accept(SocketMultiplexer& multiplexer, SOCKETHANDLE serverSocket, SocketAddress* socketAddr = nullptr);

/**
 * \brief   Accepts one pending client connection on \a serverSocket.
 *          Legacy stateless overload — builds a temporary poll list from
 *          \a masterList.  Prefer the SocketMultiplexer overload for
 *          persistent server accept loops.
 *
 * \param   serverSocket    Listening server socket descriptor.
 * \param   masterList      Array of already-accepted socket descriptors, or
 *                          nullptr if none.
 * \param   entriesCount    Number of entries in \a masterList; zero if nullptr.
 * \param   socketAddr      If not nullptr, receives the new client's address.
 * \return  Valid descriptor for the new connection; FailedSocketHandle if
 *          \a serverSocket is invalid; InvalidSocketHandle on other failure.
 **/
AREG_API SOCKETHANDLE server_accept(SOCKETHANDLE serverSocket, const SOCKETHANDLE* masterList, int32_t entriesCount, SocketAddress* socketAddr = nullptr);

/**
 * \brief   Returns the OS send-buffer size in bytes for \a hSocket.
 **/
AREG_API uint32_t max_send_size(SOCKETHANDLE hSocket);

/**
 * \brief   Sets the send-buffer size for \a hSocket to \a sendSize bytes.
 *          \a sendSize is clamped to [PACKET_MIN_SIZE, PACKET_MAX_SIZE].
 *
 * \return  The previously configured send size.
 **/
AREG_API uint32_t set_send_size(SOCKETHANDLE hSocket, uint32_t sendSize);

/**
 * \brief   Returns the OS receive-buffer size in bytes for \a hSocket.
 **/
AREG_API uint32_t max_receive_size(SOCKETHANDLE hSocket);

/**
 * \brief   Sets the receive-buffer size for \a hSocket to \a recvSize bytes.
 *          \a recvSize is clamped to [PACKET_MIN_SIZE, PACKET_MAX_SIZE].
 *
 * \return  The previously configured receive size.
 **/
AREG_API uint32_t set_recv_size(SOCKETHANDLE hSocket, uint32_t recvSize);

/**
 * \brief   Sends \a dataLength bytes from \a dataBuffer through \a hSocket.
 *          Loops internally on partial sends; returns when all bytes are sent
 *          or a connection error occurs.
 *
 * \param   hSocket         Valid connected socket descriptor.
 * \param   dataBuffer      Buffer holding the bytes to send.
 * \param   dataLength      Number of bytes to send.
 * \return  Total bytes sent (== \a dataLength on success); negative on error;
 *          0 if buffer is empty.
 **/
AREG_API int32_t send_data(SOCKETHANDLE hSocket, const uint8_t* dataBuffer, uint32_t dataLength);

/**
 * \brief   Receives up to \a dataLength bytes from \a hSocket into \a dataBuffer.
 *          Loops internally on partial receives; returns when the buffer is full,
 *          the peer closes the connection, or an error occurs.
 *
 * \param   hSocket         Valid connected socket descriptor.
 * \param   dataBuffer      Buffer to fill with received bytes.
 * \param   dataLength      Capacity of \a dataBuffer in bytes.
 * \return  Bytes received (> 0); 0 if the peer closed the connection;
 *          negative on error or empty buffer.
 **/
AREG_API int32_t receive_data(SOCKETHANDLE hSocket, uint8_t* dataBuffer, uint32_t dataLength);

/**
 * \brief   Shuts down the send direction of \a hSocket (half-close).
 *          The socket becomes receive-only.
 *
 * \return  true on success; false otherwise.
 **/
AREG_API bool disable_send(SOCKETHANDLE hSocket);

/**
 * \brief   Shuts down the receive direction of \a hSocket (half-close).
 *          The socket becomes send-only.
 *
 * \return  true on success; false otherwise.
 **/
AREG_API bool disable_receive(SOCKETHANDLE hSocket);

/**
 * \brief   Returns true if \a hSocket is still connected (not closed by
 *          the peer or the OS).
 **/
[[nodiscard]]
AREG_API bool is_socket_alive(SOCKETHANDLE hSocket);

/**
 * \brief   Returns the number of bytes queued in the receive buffer of
 *          \a hSocket and ready to read without blocking.
 **/
AREG_API uint32_t pending_read(SOCKETHANDLE hSocket);

/**
 * \brief   Returns the local machine host name, or an empty string on failure.
 **/
AREG_API const String& hostname();

/**
 * \brief   Returns true if \a address is a loopback address
 *          ("localhost" or "127.0.0.1").
 **/
[[nodiscard]] inline bool is_local_address(const String& address) noexcept;

/**
 * \brief   Returns true if \a address is a well-formed IPv4 dotted-decimal
 *          string (e.g. "192.168.0.1").
 **/
[[nodiscard]]
AREG_API bool is_ip_address(const String& address);

/**
 * \brief   Resolves a human-readable host name to an IPv4 address string.
 *          Returns \a hostName unchanged if resolution fails.
 **/
AREG_API String host_to_ip(const String& hostName);

/**
 * \brief   Resolves an IPv4 address to a host name.
 *          Returns \a ipAddress unchanged if resolution fails.
 **/
AREG_API String ip_to_host(const String& ipAddress);

/**
 * \brief   Extracts the dotted-decimal IPv4 address from \a sockAddr.
 **/
AREG_API String extract_ip_address(const struct sockaddr_in& sockAddr);

/**
 * \brief   Extracts the port number from \a sockAddr.
 **/
AREG_API uint16_t extract_port_number(const struct sockaddr_in& sockAddr);

} // namespace areg

//////////////////////////////////////////////////////////////////////////
// areg::SocketAddress — inline implementations
//////////////////////////////////////////////////////////////////////////

inline const areg::String& areg::SocketAddress::host_address() const noexcept
{
    return mIpAddr;
}

inline const areg::String& areg::SocketAddress::host_name() const noexcept
{
    return mHostName;
}

inline uint16_t areg::SocketAddress::host_port() const noexcept
{
    return mPortNr;
}

inline void areg::SocketAddress::reset() noexcept
{
    mIpAddr.clear();
    mHostName.clear();
    mPortNr = areg::InvalidPort;
}

inline bool areg::SocketAddress::is_valid() const noexcept
{
    return (!mIpAddr.is_empty()) & (mPortNr != areg::InvalidPort);
}

//////////////////////////////////////////////////////////////////////////
// areg free function — inline implementations
//////////////////////////////////////////////////////////////////////////

inline bool areg::is_valid_socket(SOCKETHANDLE hSocket) noexcept
{
    return (hSocket != areg::InvalidSocketHandle) & (hSocket != areg::FailedSocketHandle);
}

inline bool areg::is_local_address(const areg::String& address) noexcept
{
    return (address == areg::LocalHost) || (address == areg::LocalAddress);
}

#endif  // AREG_BASE_SOCKETDEFS_HPP
