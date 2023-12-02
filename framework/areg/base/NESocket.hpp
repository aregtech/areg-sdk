#ifndef AREG_BASE_NESOCKET_HPP
#define AREG_BASE_NESOCKET_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/base/NESocket.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform. Socket OS Wrapper methods
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
     * \brief   Socket address object used to resolve names to get IP-address,
     *          to get IP-address of connected socket and to create socket
     *          address structure used in socket API calls.
     **/
    class AREG_API SocketAddress
    {
    //////////////////////////////////////////////////////////////////////////
    // COnstructors / Destructor
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Default constructor creates empty IP-address and invalid port number.
         **/
        SocketAddress( void );

        /**
         * \brief   Copies the socket address data from given source.
         * \param   source  The source of data to copy.
         **/
        SocketAddress( const NESocket::SocketAddress & source );

        /**
         * \brief   Moves the socket address data from given source.
         * \param   source  The source of data to move.
         **/
        SocketAddress( SocketAddress  && source ) noexcept;

    //////////////////////////////////////////////////////////////////////////
    // Operators
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Assigning operator. Copies IP-address and port number from given source.
         * \param   source  The source of Socket Address data to copy.
         **/
        NESocket::SocketAddress & operator = ( const NESocket::SocketAddress & source );

        /**
         * \brief   Move operator. Moves IP-address and port number from given source.
         * ]param   source  The source of Socket Address data to move.
         **/
        NESocket::SocketAddress & operator = ( NESocket::SocketAddress && source ) noexcept;

        /**
         * \brief   Checks equality of 2 objects.
         * \param   other   The second object to compare
         * \return  Returns true if 2 objects are equal.
         **/
        bool operator == ( const NESocket::SocketAddress & other ) const;

        /**
         * \brief   Checks inequality of 2 objects.
         * \param   other   The second object to compare.
         * \return  Returns true if 2 objects are not equal.
         **/
        bool operator != ( const NESocket::SocketAddress & other ) const;

    //////////////////////////////////////////////////////////////////////////
    // Operations
    //////////////////////////////////////////////////////////////////////////
    public:

        /**
         * \brief   Converts existing address to sockaddr_in structure.
         *          If IP-address and port numbers are valid, on output
         *          the passed out_sockAddr contains valid data.
         * \param   out_sockAddr    The instance of sockaddr_in to fill data.
         * \return  Returns true if succeeded to convert human readable string
         *          IP-address and port number to sockaddr_in structure data.
         **/
        bool getAddress( struct sockaddr_in & out_sockAddr ) const;

        /**
         * \brief   Sets host IP-address and port number extracted from given
         *          Internet address structure.
         * \param   addrHost    The structure containing host address and port number.
         **/
        void setAddress( const struct sockaddr_in & addrHost );

        /**
         * \brief   Resolves passed name to IP-address and saves in data.
         *          If hostName is an IP-address, it will not be resolved and will be saved
         *          as it is. No additional work is done for passed port number.
         *          The flag isServer is indicating whether it should solve address
         *          for client or server socket. The server socket supposed to be used
         *          for binding with created socket.
         * \param   hostName    The numeric IP-address or host name to resolve.
         *                      If nullptr, the localhost will be resolved.
         * \param   portNr      The port number to resolve.
         * \param   isServer    The flag, indicating whether it should resolve for client
         *                      or server socket. The server socket supposed to bind.
         * \return  Returns true if succeeded to resolve name.
         **/
        bool resolveAddress( const std::string_view & hostName, unsigned short portNr, bool isServer );

        /**
         * \brief   Resolves and retrieves the address of the peer to which a socket is connected.
         *          The function can be used only on a connected socket.
         * \param   hSocket     The socket descriptor of connected socket.
         * \return  Returns true if succeeded to resolve address of connected peer.
         **/
        bool resolveSocket( SOCKETHANDLE hSocket );

        /**
         * \brief   Returns IP address of host as readable string.
         **/
        inline const String & getHostAddress( void ) const;

        /**
         * \brief   Returns port number of host.
         **/
        inline unsigned short getHostPort( void ) const;

        /**
         * \brief   Resets the IP-address and port number of address
         **/
        inline void resetAddress( void );

        /**
         * \brief   Returns true if IP-address is not empty and port number is valid.
         **/
        inline bool isValid( void ) const;

    //////////////////////////////////////////////////////////////////////////
    // Member variables
    //////////////////////////////////////////////////////////////////////////
    private:
        /**
         * \brief   The string containing human readable numeric IP-address.
         **/
        String          mIpAddr;
        /**
         * \brief   The port number of socket to connect.
         **/
        unsigned short  mPortNr;
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
    constexpr unsigned short            InvalidPort                 { 0 };
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
    constexpr unsigned int              PACKET_MIN_SIZE             { 512 };

    /**
     * \brief   NESocket::PACKET_DEFAULT_SIZE
     *          The default size of packet when send or receive data.
     **/
    constexpr unsigned int              PACKET_DEFAULT_SIZE         { 1500 };

    /**
     * \brief   NESocket::PACKET_MAX_SIZE
     *          The maximum size of packet when send or receive data.
     **/
    constexpr unsigned int              PACKET_MAX_SIZE             { 65536 };

    /**
     * \brief   NESocket::PACKET_INVALID_SIZE
     *          Packet invalid size.
     **/
    constexpr unsigned int              PACKET_INVALID_SIZE         { 0 };

    /**
     * \brief   NESocket::MAXIMUM_LISTEN_QUEUE_SIZE
     *          Constant, identifying maximum number of listeners in the queue.
     *          Used by server socket when set to listen connection.
     **/
    extern AREG_API const int           MAXIMUM_LISTEN_QUEUE_SIZE   /*= SOMAXCONN*/;

//////////////////////////////////////////////////////////////////////////
// NESocket namespace functions
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   NESocket::isSocketHandleValid
     *          Checks socket descriptor and returns true if it not equal to InvalidSocketHandle.
     * \param   hSocket     The socket descriptor to check
     **/
    inline bool isSocketHandleValid( SOCKETHANDLE hSocket );

    /**
     * \brief   NESocket::socketInitialize
     *          Call to initialize socket in the current process.
     *          Function should be called before any socket operation is performed.
     *          Otherwise, all socket operations might fail.
     *          Note:   The method will initialize socket in the current process only
     *                  by first call. All other calls will have no effect until socket
     *                  is not released. The number of release calls should be equal to
     *                  number of initialization.
     * \return  Returns true if socket is initialized and socket function might be called
     *          in the current process.
     **/
    AREG_API bool socketInitialize( void );

    /**
     * \brief   NESocket::socketRelease
     *          Call to release socket and free resources in current process
     *          The call is not releasing socket until number of SocketRelease() calls
     *          is not equal to number of SocketInitialize() calls.
     **/
    AREG_API void socketRelease( void );

    /**
     * \brief   NESocket::socketCreate
     *          Creates streaming TCP/IP socket for client or server.
     * \return  If function succeeds, returns valid socket descriptor not equal to
     *          NESocket::InvalidSocketHandle.
     **/
    AREG_API SOCKETHANDLE socketCreate( void );

    /**
     * \brief   NESocket::socketClose
     *          Closes passed socket descriptor. No data sending or receiving via
     *          specified socket will be possible.
     * \param   hSocket     The socket to close.
     **/
    AREG_API void socketClose( SOCKETHANDLE hSocket );

    /**
     * \brief   NESocket::clientSocketConnect
     *          Creates client TCP/IP socket and connect to specified peer address.
     * \param   peerAddr    The object containing remote host IP-address and port number.
     * \return  Returns valid socket descriptor, if could create socket and connect to remote peer.
     *          Otherwise, it returns NESocket::InvalidSocketHandle value.
     **/
    AREG_API SOCKETHANDLE clientSocketConnect( const NESocket::SocketAddress & peerAddr );

    /**
     * \brief   NESocket::clientSocketConnect
     *          Creates client TCP/IP socket and connect to specified remote host name and port number.
     *          The host name can be either numeric IP-address or human readable host name to resolve.
     *          If passed out_socketAddr pointer is not nullptr, on output it will contain readable
     *          numeric IP-address with dots and port number (IP-address like "123.45.678.90"
     * \param   hostName    The host name or IP_address of remote server to connect.
     * \param   portNr      The port number to connect. This should be valid port number.
     * \return  Returns valid socket descriptor, if could create socket and connect to remote peer.
     *          Otherwise, it returns NESocket::InvalidSocketHandle value.
     **/
    AREG_API SOCKETHANDLE clientSocketConnect( const std::string_view & hostName, unsigned short portNr, NESocket::SocketAddress * out_socketAddr = nullptr );

    /**
     * \brief   NESocket::serverSocketConnect
     *          Creates server TCP/IP socket and binds to specified server address.
     *          Before accepting any connection, the socket should be set for listening.
     * \param   peerAddr    The object containing host IP-address and port number of server.
     * \return  Returns valid socket descriptor, if could create socket and bind to specified address.
     *          Otherwise, it returns NESocket::InvalidSocketHandle value.
     **/
    AREG_API SOCKETHANDLE serverSocketConnect( const NESocket::SocketAddress & peerAddr );

    /**
     * \brief   NESocket::serverSocketConnect
     *          Creates server TCP/IP socket and binds to specified host name and port number.
     *          Before accepting any connection, the socket should be set for listening.
     *          The host name can be either numeric IP-address or human readable host name to resolve.
     *          If passed out_socketAddr pointer is not nullptr, on output it will contain readable
     *          numeric IP-address with dots and port number (IP-address like "123.45.678.90"
     * \param   hostName    The host name or server IP-address to bind.
     * \param   portNr      The port number to bind. This should be valid port number.
     * \return  Returns valid socket descriptor, if could create socket and bind to specified address.
     *          Otherwise, it returns NESocket::InvalidSocketHandle value.
     **/
    AREG_API SOCKETHANDLE serverSocketConnect( const std::string_view & hostName, unsigned short portNr, NESocket::SocketAddress * out_socketAddr = nullptr );

    /**
     * \brief   NESocket::serverListenConnection
     *          Called by server. Sets specified valid server socket for listening incoming connections.
     *          Note:   Before calling this method, the server socket should be created and bind.
     * \param   serverSocket    The valid socket descriptor of server.
     * \param   maxQueueSize    The maximum number of queue.
     * \return  Returns true if server is listening and ready to accept connections.
     **/
    AREG_API bool serverListenConnection( SOCKETHANDLE serverSocket, int maxQueueSize = NESocket::MAXIMUM_LISTEN_QUEUE_SIZE);

    /**
     * \brief   NESocket::serverAcceptConnection
     *          Called by server when starts to accept connections.
     *          Note:   Before calling this method, the server socket should be created, bind and
     *                  should be in listening mode.
     * \param   serverSocket    The valid socket descriptor of server.
     * \param   masterList      The list of previously accepted connections.
     *                          Or nullptr if server has no accepted connection yet.
     * \param   entriesCount    The number of entries in specified accepted list. 
     *                          Or Zero if server has no accepted connection yet.
     * \param   out_socketAddr  If not nullptr and new connection is accepted, on output this will contain
     *                          the IP address and port number of new accepted connection.
     *                          Note:   the data will be valid only in case of new accepted connections.
     *                                  In all other cases there will be no valid data.
     * \return  If succeeds to accept connection, returns valid accepted socket descriptor.
     *          If server socket is not valid anymore, returns NESocket::FailedSocketHandle.
     *          In all other failure cases, returns NESocket::InvalidSocketHandle.
     **/
    AREG_API SOCKETHANDLE serverAcceptConnection( SOCKETHANDLE serverSocket, const SOCKETHANDLE * masterList, int entriesCount, NESocket::SocketAddress * out_socketAddr = nullptr );

    /**
     * \brief   NESocket::getMaxSendSize
     *          Returns the socket buffer size in bytes to send the packet at once.
     *          This value may vary by protocol.
     * \param   hSocket     The valid socket descriptor to retrieve value.
     **/
    AREG_API unsigned int getMaxSendSize( SOCKETHANDLE hSocket );

    /**
     * \brief   NESocket::setMaxSendSize
     *          Sets the socket packet size in bytes to send data at once.
     * \param   hSocket     The valid socket descriptor to set the value.
     * \param   sendSize    The size in bytes to sent the packet at once.
     *                      The minimum size is NESocket::PACKET_MIN_SIZE,
     *                      the maximum is NESocket::PACKET_MAX_SIZE.
     **/
    AREG_API unsigned int setMaxSendSize(SOCKETHANDLE hSocket, unsigned int sendSize);

    /**
     * \brief   NESocket::getMaxReceiveSize
     *          Maximum size of packet in bytes to receive data.
     * \param   hSocket     The valid socket descriptor to retrieve value.
     **/
    AREG_API unsigned int getMaxReceiveSize( SOCKETHANDLE hSocket );

    /**
     * \brief   NESocket::setMaxReceiveSize
     *          Sets the socket packet size in bytes to receive data at once.
     * \param   hSocket     The valid socket descriptor to set the value.
     * \param   recvSize    The size in bytes to receive the packet at once.
     *                      The minimum size is NESocket::PACKET_MIN_SIZE.
     *                      The maximum is NESocket::PACKET_MAX_SIZE.
     *                      The default seize is NESocket::PACKET_DEFAULT_SIZE.
     **/
    AREG_API unsigned int setMaxReceiveSize(SOCKETHANDLE hSocket, unsigned int recvSize);

    /**
     * \brief   NESocket::sendData
     *          Send data to specified socket. The passed socket descriptor should be valid.
     *          If blockMaxSize value is negative, it will retrieve the maximum number of package size
     *          to send at once and try to send data.
     * \param   hSocket         The valid socket descriptor to send data.
     * \param   dataBuffer      The pointer to data buffer, which should be sent.
     * \param   dataLength      The length of buffer in bytes.
     * \param   blockMaxSize    The maximum size of package in bytes to sent at once.
     *                          If negative or zero value, it will first retrieve value and sent data.
     * \return  If succeeds, returns number of bytes sent.
     *          If failles, returns negative number.
     *          Returns zero if buffer is empty and nothing to sent.
     **/
    AREG_API int sendData( SOCKETHANDLE hSocket, const unsigned char * dataBuffer, int dataLength, int blockMaxSize );

    /**
     * \brief   NESocket::receiveData
     *          Receives data on specified socket. The passed socket descriptor should be valid.
     *          If blockMaxSize value is negative, it will retrieve the maximum number of package size
     *          to receive at once.
     * \param   hSocket         The valid socket descriptor to receive data.
     * \param   dataBuffer      The pointer to data buffer, which should be filled.
     * \param   dataLength      The length of buffer in bytes.
     * \param   blockMaxSize    The maximum size of package in bytes to receive at once.
     *                          If negative or zero value, it will first retrieve value and start receive data.
     * \return  If succeeds, returns number of bytes received.
     *          If failles, returns negative number. Failure might happen if opposite side closes connection.
     *          In case of failure, the specified socket should be closed.
     *          Returns zero if buffer is empty and nothing to receive.
     **/
    AREG_API int receiveData( SOCKETHANDLE hSocket, unsigned char * dataBuffer, int dataLength, int blockMaxSize );

    /**
     * \brief   NESocket::disableSend
     *          Sets socket read-only, i.e. it will not be possible to send messages anymore.
     * \param   hSocket     Valid socket handle to set in read-only mode, i.e. no send message is possible anymore.
     * \return  Returns true if operation succeeds. Otherwise, returns false.
     **/
    AREG_API bool disableSend( SOCKETHANDLE hSocket );

    /**
     * \brief   NESocket::disableReceive
     *          Sets socket write-only, i.e. it will not be possible to receive messages anymore.
     * \param   hSocket     Valid socket handle to set in read-only mode, i.e. no receive is possible anymore.
     * \return  Returns true if operation succeeds. Otherwise, returns false.
     **/
    AREG_API bool disableReceive( SOCKETHANDLE hSocket );

    /**
     * \brief   Checks and returns socket alive state.
     * \param   hSocket     The socked handle to check.
     * \return  Returns true if specified socket is alive and is not closed.
     **/
    AREG_API bool isSocketAlive( SOCKETHANDLE hSocket );

    /**
     * \brief   NESocket::pendingRead
     *          Checks and returns number of bytes remain to read from socket buffer.
     * \param   hSocket     The socket handle to check.
     * \return  Returns number of bytes available to read from specified socket buffer.
     **/
    AREG_API unsigned int pendingRead( SOCKETHANDLE hSocket );

    /**
     * \brief   NESocket::getHostname
     *          Returns the host name of the local machine.
     *          Or returns empty string if failed.
     **/
    AREG_API const String & getHostname(void);

}   // namespace NESocket end

//////////////////////////////////////////////////////////////////////////
// NESocket::SocketAddress class inline function
//////////////////////////////////////////////////////////////////////////

inline bool NESocket::SocketAddress::isValid( void ) const
{
    return ((mIpAddr.isEmpty() == false) && (mPortNr != NESocket::InvalidPort));
}

inline const String & NESocket::SocketAddress::getHostAddress( void ) const
{
    return mIpAddr;
}

inline unsigned short NESocket::SocketAddress::getHostPort( void ) const
{
    return mPortNr;
}

inline void NESocket::SocketAddress::resetAddress( void )
{
    mIpAddr = String::EmptyString;
    mPortNr = NESocket::InvalidPort;
}

inline bool NESocket::isSocketHandleValid(SOCKETHANDLE hSocket)
{
    return ((hSocket != NESocket::InvalidSocketHandle) && (hSocket != NESocket::FailedSocketHandle));
}

#endif  // AREG_BASE_NESOCKET_HPP
