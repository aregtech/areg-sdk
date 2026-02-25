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
 * \brief       Areg Platform. Socket OS Wrapper methods
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/String.hpp"

#include <string_view>

/************************************************************************
 * Dependencies
 ************************************************************************/
struct sockaddr_in;

/**
 * \brief   NESocket namespace is a wrapper of basic socket functionalities
 *          like create and close socket, connect client and server, accept
 *          client connect for server sockets, send and receive data via socket.
 *          The purpose of this namespace is wrapping OS depended socket functions.
 *          The namespace also contains basic constants initialization and
 *          Socket Address class, which is used to store IP address and port
 *          number for socket connection. The Socket Address object also used
 *          to resolve names and get connected peer address.
 *
 * \note    Currently the existing socket functionalities support only TCP/IP
 *          connection for IP4 addresses. All other connection types
 *          are ignored and out of scope of this namespace
 **/
namespace NESocket
{
//////////////////////////////////////////////////////////////////////////
// NESocket::SocketAddress class declaration
//////////////////////////////////////////////////////////////////////////
    /**
     * \brief   Manages socket address information (IP address and port); resolves names to IP
     *          addresses.
     **/
    class AREG_API SocketAddress
    {
    //////////////////////////////////////////////////////////////////////////
    // COnstructors / Destructor
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Creates an empty socket address with an invalid port.
         **/
        SocketAddress();

        /**
         * \brief   Initializes the socket address with an IP address and port number.
         *
         * \param   address     The IP address of the socket.
         * \param   portNr      The port number of the socket.
         **/
        SocketAddress(const String& address, uint16_t portNr);

        /**
         * \brief   Copies the socket address from the source.
         *
         * \param   source      The source socket address to copy from.
         **/
        SocketAddress( const NESocket::SocketAddress & source );

        /**
         * \brief   Moves the socket address from the source.
         *
         * \param   source      The source socket address to move from.
         **/
        SocketAddress( SocketAddress && source ) noexcept;

    //////////////////////////////////////////////////////////////////////////
    // Operators
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Assigns the IP address and port from the source.
         *
         * \param   source      The source socket address to copy from.
         * \return  Reference to this socket address.
         **/
        NESocket::SocketAddress & operator = ( const NESocket::SocketAddress & source );

        /**
         * \brief   Moves the IP address and port from the source.
         *
         * \param   source      The source socket address to move from.
         * \return  Reference to this socket address.
         **/
        NESocket::SocketAddress & operator = ( NESocket::SocketAddress && source ) noexcept;

        /**
         * \brief   Returns true if two socket addresses are equal.
         *
         * \param   other       The socket address to compare.
         * \return  Returns true if both addresses are equal; false otherwise.
         **/
        bool operator == ( const NESocket::SocketAddress & other ) const;

        /**
         * \brief   Returns true if two socket addresses are not equal.
         *
         * \param   other       The socket address to compare.
         * \return  Returns true if the addresses are not equal; false otherwise.
         **/
        bool operator != ( const NESocket::SocketAddress & other ) const;

    //////////////////////////////////////////////////////////////////////////
    // Operations
    //////////////////////////////////////////////////////////////////////////
    public:

        /**
         * \brief   Converts the socket address to a sockaddr_in structure.
         *
         * \param[out] out_sockAddr    The sockaddr_in structure to fill with converted address
         *                             data.
         * \return  Returns true if the conversion succeeded; false otherwise.
         **/
        bool address( struct sockaddr_in & out_sockAddr ) const;

        /**
         * \brief   Sets the IP address and port from a sockaddr_in structure.
         *
         * \param   addrHost    The sockaddr_in structure containing the address and port.
         **/
        void set_address( const struct sockaddr_in & addrHost );

        /**
         * \brief   Resolves a host name to an IP address and sets the port number.
         *
         * \param   hostName    The numeric IP address or host name to resolve; if nullptr,
         *                      localhost is used.
         * \param   portNr      The port number to set.
         * \param   isServer    If true, resolves for server socket binding; if false, for client
         *                      socket connection.
         * \return  Returns true if the name resolution succeeded; false otherwise.
         **/
        bool resolve_address( const std::string_view & hostName, uint16_t portNr, bool isServer );

        /**
         * \brief   Resolves and stores the address of the peer connected to the given socket.
         *
         * \param   hSocket     The socket descriptor of a connected socket.
         * \return  Returns true if the address resolution succeeded; false otherwise.
         **/
        bool resolve_socket( SOCKETHANDLE hSocket );

        /**
         * \brief   Returns true if the stored address matches the given host and port.
         *
         * \param   host    The IPv4 address or host name to compare.
         * \param   port    The port number to compare.
         * \return  Returns true if both the host and port match; false otherwise.
         **/
        bool is_equal_address(const String& host, uint16_t port) const;

        /**
         * \brief   Returns the IP address as a readable string.
         **/
        inline const String & host_address() const;

        /**
         * \brief   Returns the host name as a readable string.
         **/
        inline const String & host_name() const;

        /**
         * \brief   Returns the port number.
         **/
        inline uint16_t host_port() const;

        /**
         * \brief   Clears the IP address and port number.
         **/
        inline void reset();

        /**
         * \brief   Returns true if the IP address and port number are valid.
         **/
        inline bool is_valid() const;

    //////////////////////////////////////////////////////////////////////////
    // Member variables
    //////////////////////////////////////////////////////////////////////////
    private:
        /**
         * \brief   The string containing human readable numeric IP-address.
         **/
        String          mIpAddr;
        /**
         * \brief   The string contains human readable host name.
         **/
        String          mHostName;
        /**
         * \brief   The port number of socket to connect.
         **/
        uint16_t  mPortNr;
    };

//////////////////////////////////////////////////////////////////////////
// NESocket::UserData class declaration
//////////////////////////////////////////////////////////////////////////
    /**
     * \brief   Container for a user name and password pair.
     **/
    class AREG_API UserData
    {
    //////////////////////////////////////////////////////////////////////////
    // Constructors
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief
         **/
        UserData();

        /**
         * \brief   Initializes user name and password with the specified values.
         *
         * \param   user        The user name.
         * \param   password    The password.
         **/
        UserData(const String& user, const String& password);

        /**
         * \brief
         *
         * \param   src     The source to copy.
         **/
        UserData(const UserData& src);
        /**
         * \brief
         *
         * \param   src     The source to move.
         * \note    Move constructor.
         **/
        UserData(UserData&& src) noexcept;

    //////////////////////////////////////////////////////////////////////////
    // Operators
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief
         *
         * \param   source      The source to copy.
         **/
        NESocket::UserData& operator = (const NESocket::UserData& source);
        /**
         * \brief
         *
         * \param   source      The source to move.
         * \note    Move assignment operator.
         **/
        NESocket::UserData& operator = (NESocket::UserData&& source) noexcept;

        /**
         * \brief   Returns true if user name and password are equal.
         *
         * \param   other       The other UserData to compare.
         **/
        bool operator == (const NESocket::UserData & other);

        /**
         * \brief   Returns true if user name or password are not equal.
         *
         * \param   other       The other UserData to compare.
         **/
        bool operator != (const NESocket::UserData & other);

        /**
         * \brief
         **/
        const String& user() const;

        /**
         * \brief
         *
         * \param   user    The user name to set.
         **/
        void set_user(const String& user);

        /**
         * \brief
         **/
        const String& password() const;

        /**
         * \brief
         *
         * \param   password    The password to set.
         **/
        void set_password(const String& password);

        /**
         * \brief   Returns true if the entry is valid (at least the user name is not empty).
         **/
        bool is_valid() const;

    //////////////////////////////////////////////////////////////////////////
    // Member variables
    //////////////////////////////////////////////////////////////////////////
    private:
        //!< The user name entry.
        String mUser;
        //!< The user password entry.
        String mPassword;
    };

//////////////////////////////////////////////////////////////////////////
// NESocket namespace constants declaration
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   NESocket::InvalidSocketHandle
     *          Constant, identifying invalid socket descriptor.
     **/
    constexpr const SOCKETHANDLE        InvalidSocketHandle         { static_cast<SOCKETHANDLE>(~0) };
    /**
     * \brief   NESocket::FailedSocketHandle
     *          Invalid connection socket descriptor. Used to indicate failure on server socket.
     **/
    constexpr const SOCKETHANDLE        FailedSocketHandle          { static_cast<SOCKETHANDLE>(~1) };
    /**
     * \brief   NESocket::InvalidSocket
     *          Constant, identifying invalid port number
     **/
    constexpr uint16_t            InvalidPort                 { 0 };
    /**
     * \brief   NESocket::LocalHost
     *          Constant, identifying local host
     **/
    constexpr std::string_view          LocalHost                   { "localhost" };
    /**
     * \brief   NESocket::LocalAddress
     *          Constant, identifying local IP address
     **/
    constexpr std::string_view          LocalAddress                { "127.0.0.1" };
    /**
     * \brief   NESocket::IP_SEPARATOR
     *          The property separator
     **/
    constexpr char              IP_SEPARATOR                        { '.' };
    /**
     * \brief   NESocket::IP_ADDRESS_SIZE
     *          The size of buffer to reserve for IP address, like "255.255.255.255"
     **/
    constexpr uint32_t  IP_ADDRESS_SIZE                             { 16 };

    /**
     * \brief   NESocket::PACKET_MIN_SIZE
     *          The minimum size of packet when send or receive data.
     **/
    constexpr uint32_t              PACKET_MIN_SIZE             { 512 };

    /**
     * \brief   NESocket::PACKET_DEFAULT_SIZE
     *          The default size of packet when send or receive data.
     **/
    constexpr uint32_t              PACKET_DEFAULT_SIZE         { 1500 };

    /**
     * \brief   NESocket::PACKET_MAX_SIZE
     *          The maximum size of packet when send or receive data.
     **/
    constexpr uint32_t              PACKET_MAX_SIZE             { 65536 };

    /**
     * \brief   NESocket::PACKET_INVALID_SIZE
     *          Packet invalid size.
     **/
    constexpr uint32_t              PACKET_INVALID_SIZE         { 0 };

    /**
     * \brief   NESocket::MAXIMUM_LISTEN_QUEUE_SIZE
     *          Constant, identifying maximum number of listeners in the queue.
     *          Used by server socket when set to listen connection.
     **/
    extern AREG_API const int32_t           MAXIMUM_LISTEN_QUEUE_SIZE   /*= SOMAXCONN*/;

//////////////////////////////////////////////////////////////////////////
// NESocket namespace functions
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Returns true if the socket descriptor is valid (not equal to InvalidSocketHandle).
     *
     * \param   hSocket     The socket descriptor to check.
     * \return  True if the socket handle is valid; false otherwise.
     **/
    inline bool is_handle_valid( SOCKETHANDLE hSocket );

    /**
     * \brief   Initializes sockets in the current process. Must be called before any socket
     *          operation. Subsequent calls have no effect until socket_release() is called an equal
     *          number of times.
     *
     * \return  True if sockets initialized successfully; false otherwise.
     **/
    AREG_API bool socket_initialize();

    /**
     * \brief   Releases sockets and frees resources in the current process. Must be called an equal
     *          number of times as socket_initialize().
     **/
    AREG_API void socket_release();

    /**
     * \brief   Creates a streaming TCP/IP socket for client or server use.
     *
     * \return  Valid socket descriptor if successful; InvalidSocketHandle otherwise.
     **/
    AREG_API SOCKETHANDLE socket_create();

    /**
     * \brief   Closes the specified socket. No data sending or receiving will be possible
     *          afterward.
     *
     * \param   hSocket     The socket descriptor to close.
     **/
    AREG_API void socket_close( SOCKETHANDLE hSocket );

    /**
     * \brief   Creates a client TCP/IP socket and connects to the specified peer address.
     *
     * \param   peerAddr    The address object containing remote host IP and port.
     * \return  Valid socket descriptor if successful; InvalidSocketHandle if failed.
     **/
    AREG_API SOCKETHANDLE client_socket_connect( const NESocket::SocketAddress & peerAddr );

    /**
     * \brief   Creates a client TCP/IP socket and connects to the specified host name and port.
     *          Host name can be numeric IP or human-readable name.
     *
     * \param   hostName    The host name or IP address of the remote server.
     * \param   portNr      The port number to connect to.
     * \return  Valid socket descriptor if successful; InvalidSocketHandle if failed.
     **/
    AREG_API SOCKETHANDLE client_socket_connect( const std::string_view & hostName, uint16_t portNr, NESocket::SocketAddress * out_socketAddr = nullptr );

    /**
     * \brief   Creates a server TCP/IP socket and binds to the specified address. Call
     *          server_listen_connection() before accepting connections.
     *
     * \param   peerAddr    The address object containing server host IP and port.
     * \return  Valid socket descriptor if successful; InvalidSocketHandle if failed.
     **/
    AREG_API SOCKETHANDLE server_socket_connect( const NESocket::SocketAddress & peerAddr );

    /**
     * \brief   Creates a server TCP/IP socket and binds to the specified host name and port. Call
     *          server_listen_connection() before accepting connections.
     *
     * \param   hostName    The host name or server IP address to bind to.
     * \param   portNr      The port number to bind to.
     * \return  Valid socket descriptor if successful; InvalidSocketHandle if failed.
     **/
    AREG_API SOCKETHANDLE server_socket_connect( const std::string_view & hostName, uint16_t portNr, NESocket::SocketAddress * out_socketAddr = nullptr );

    /**
     * \brief   Sets the server socket to listen mode for incoming connections. Server socket must
     *          be created and bound before calling.
     *
     * \param   serverSocket    The valid server socket descriptor.
     * \param   maxQueueSize    The maximum number of pending connections in the queue.
     * \return  True if listening; false otherwise.
     **/
    AREG_API bool server_listen_connection( SOCKETHANDLE serverSocket, int32_t maxQueueSize = NESocket::MAXIMUM_LISTEN_QUEUE_SIZE);

    /**
     * \brief   Accepts a new client connection on the server socket. Server socket must be created,
     *          bound, and in listening mode.
     *
     * \param   serverSocket        The valid server socket descriptor.
     * \param   masterList          List of previously accepted connections, or nullptr if none.
     * \param   entriesCount        Number of entries in the accepted list, or zero if none.
     * \param[out] out_socketAddr      If not nullptr, receives the IP address and port of the newly
     *                                 accepted connection.
     * \return  Valid socket descriptor if a new connection accepted; FailedSocketHandle if server
     *          socket is invalid; InvalidSocketHandle for other failures.
     **/
    AREG_API SOCKETHANDLE server_accept_connection( SOCKETHANDLE serverSocket, const SOCKETHANDLE * masterList, int32_t entriesCount, NESocket::SocketAddress * out_socketAddr = nullptr );

    /**
     * \brief   Returns the socket send buffer size in bytes for each packet. Value varies by
     *          protocol.
     *
     * \param   hSocket     The valid socket descriptor.
     * \return  The maximum send packet size in bytes.
     **/
    AREG_API uint32_t max_send_size( SOCKETHANDLE hSocket );

    /**
     * \brief   Sets the maximum packet size in bytes to send at once. Size must be between
     *          PACKET_MIN_SIZE and PACKET_MAX_SIZE.
     *
     * \param   hSocket     The valid socket descriptor.
     * \param   sendSize    The size in bytes. Must be between PACKET_MIN_SIZE and PACKET_MAX_SIZE.
     * \return  The previously set send size.
     **/
    AREG_API uint32_t set_send_size(SOCKETHANDLE hSocket, uint32_t sendSize);

    /**
     * \brief   Returns the maximum packet size in bytes to receive.
     *
     * \param   hSocket     The valid socket descriptor.
     * \return  The maximum receive packet size in bytes.
     **/
    AREG_API uint32_t max_receive_size( SOCKETHANDLE hSocket );

    /**
     * \brief   Sets the maximum packet size in bytes to receive at once. Size must be between
     *          PACKET_MIN_SIZE and PACKET_MAX_SIZE.
     *
     * \param   hSocket     The valid socket descriptor.
     * \param   recvSize    The size in bytes. Must be between PACKET_MIN_SIZE and PACKET_MAX_SIZE.
     * \return  The previously set receive size.
     **/
    AREG_API uint32_t set_recv_size(SOCKETHANDLE hSocket, uint32_t recvSize);

    /**
     * \brief   Sends data through the socket. If blockMaxSize is non-positive, the maximum packet
     *          size is retrieved automatically.
     *
     * \param   hSocket         The valid socket descriptor.
     * \param   dataBuffer      The data buffer to send.
     * \param   dataLength      The length of the data in bytes.
     * \param   blockMaxSize    The maximum packet size. If non-positive, the value is retrieved
     *                          automatically.
     * \return  Number of bytes sent if successful; negative if failed; zero if buffer is empty.
     **/
    AREG_API int32_t send_data( SOCKETHANDLE hSocket, const uint8_t * dataBuffer, uint32_t dataLength, uint32_t blockMaxSize );

    /**
     * \brief   Receives data from the socket. If blockMaxSize is non-positive, the maximum packet
     *          size is retrieved automatically.
     *
     * \param   hSocket         The valid socket descriptor.
     * \param[out] dataBuffer      The buffer to fill with received data.
     * \param   dataLength      The length of the buffer in bytes.
     * \param   blockMaxSize    The maximum packet size. If non-positive, the value is retrieved
     *                          automatically.
     * \return  Number of bytes received if successful; negative if failed or connection closed;
     *          zero if buffer is empty.
     **/
    AREG_API int32_t receive_data( SOCKETHANDLE hSocket, uint8_t * dataBuffer, uint32_t dataLength, uint32_t blockMaxSize );

    /**
     * \brief   Disables sending on the socket. The socket becomes read-only.
     *
     * \param   hSocket     The valid socket descriptor.
     * \return  True if successful; false otherwise.
     **/
    AREG_API bool disable_send( SOCKETHANDLE hSocket );

    /**
     * \brief   Disables receiving on the socket. The socket becomes write-only.
     *
     * \param   hSocket     The valid socket descriptor.
     * \return  True if successful; false otherwise.
     **/
    AREG_API bool disable_receive( SOCKETHANDLE hSocket );

    /**
     * \brief   Checks whether the socket is alive and has not been closed.
     *
     * \param   hSocket     The socket descriptor to check.
     * \return  True if the socket is alive; false if closed.
     **/
    AREG_API bool is_socket_alive( SOCKETHANDLE hSocket );

    /**
     * \brief   Returns the number of bytes available to read from the socket buffer.
     *
     * \param   hSocket     The socket descriptor to check.
     * \return  Number of bytes pending to read.
     **/
    AREG_API uint32_t pending_read( SOCKETHANDLE hSocket );

    /**
     * \brief   Returns the host name of the local machine, or empty string if the operation fails.
     *
     * \return  The local host name.
     **/
    AREG_API const String & hostname();

    /**
     * \brief   Returns true if the IP address is a local address ("localhost" or "127.0.0.1").
     *
     * \param   ipaddress       The IP address to check.
     * \return  True if the address is local; false otherwise.
     **/
    inline bool is_local_address(const String& ipaddress);

    /**
     * \brief   Returns true if the string contains a valid IPv4 address.
     *
     * \param   ipaddress       The string to check.
     * \return  True if a valid IPv4 address like "127.0.0.1"; false otherwise.
     **/
    AREG_API bool is_ip_address(const String& ipaddress);

    /**
     * \brief   Converts a host name to an IPv4 address, or returns the original hostName if
     *          conversion fails.
     *
     * \param   hostName    The human-readable host name to convert.
     * \return  The IPv4 address, or the original hostName if conversion fails.
     **/
    AREG_API String host_to_ip(const String& hostName);

    /**
     * \brief   Converts an IPv4 address to a host name, or returns the original ipAddress if
     *          conversion fails.
     *
     * \param   ipAddress       The IPv4 address to convert.
     * \return  The host name, or the original ipAddress if conversion fails.
     **/
    AREG_API String ip_to_host(const String& ipAddress);

    /**
     * \brief   Extracts the IPv4 address from a sockaddr_in structure.
     *
     * \param   addrHost    The sockaddr_in structure containing the IP address.
     * \return  Human-readable IPv4 address.
     **/
    AREG_API String extract_ip_address(const struct sockaddr_in& addrHost);

    /**
     * \brief   Extracts the port number from a sockaddr_in structure.
     *
     * \param   addrHost    The sockaddr_in structure containing the port number.
     * \return  The port number.
     **/
    AREG_API uint16_t extract_port_number(const struct sockaddr_in& addrHost);

}   // namespace NESocket end

//////////////////////////////////////////////////////////////////////////
// NESocket::SocketAddress class inline function
//////////////////////////////////////////////////////////////////////////

inline bool NESocket::SocketAddress::is_valid() const
{
    return ((mIpAddr.is_empty() == false) && (mPortNr != NESocket::InvalidPort));
}

inline const String & NESocket::SocketAddress::host_address() const
{
    return mIpAddr;
}

inline const String& NESocket::SocketAddress::host_name() const
{
    return mHostName;
}

inline uint16_t NESocket::SocketAddress::host_port() const
{
    return mPortNr;
}

inline void NESocket::SocketAddress::reset()
{
    mIpAddr.clear();
    mHostName.clear();
    mPortNr = NESocket::InvalidPort;
}

inline bool NESocket::is_handle_valid(SOCKETHANDLE hSocket)
{
    return ((hSocket != NESocket::InvalidSocketHandle) && (hSocket != NESocket::FailedSocketHandle));
}

inline bool NESocket::is_local_address(const String& ipaddress)
{
    return (ipaddress == NESocket::LocalHost) || (ipaddress == NESocket::LocalAddress);
}

#endif  // AREG_BASE_SOCKETDEFS_HPP
