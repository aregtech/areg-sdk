#ifndef AREG_BASE_NESOCKET_HPP
#define AREG_BASE_NESOCKET_HPP

/************************************************************************
 * \file        areg/base/NESocket.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform. Socket OS Wrapper methods
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/String.hpp"

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
// NESocket::InterlockedValue class declaration
//////////////////////////////////////////////////////////////////////////
    /**
     * \brief   Socket address object used to resolve names to get IP-address,
     *          to get IP-address of connected socket and to create socket
     *          address structure used in socket API calls.
     **/
    class AREG_API InterlockedValue
    {
    //////////////////////////////////////////////////////////////////////////
    // COnstructors / Destructor
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Default constructor. 
         *          Creates empty IP-address and invalid port number
         **/
        InterlockedValue( void );

        /**
         * \brief   Copy constructor.
         * \param   source  The source to copy data.
         **/
        InterlockedValue( const NESocket::InterlockedValue & source );

    //////////////////////////////////////////////////////////////////////////
    // Operators
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Assigning operator. Copies IP-address and port number from given source.
         * \param   source  The source of Socket Address to copy data.
         **/
        const NESocket::InterlockedValue & operator = ( const NESocket::InterlockedValue & source );

        /**
         * \brief   Checks equality of 2 objects.
         * \param   other   The second object to compare
         * \return  Returns true if 2 objects are equal.
         **/
        bool operator == ( const NESocket::InterlockedValue & other ) const;

        /**
         * \brief   Checks inequality of 2 objects.
         * \param   other   The second object to compare.
         * \return  Returns true if 2 objects are not equal.
         **/
        bool operator != ( const NESocket::InterlockedValue & other ) const;

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
         *                      If NULL, the localhost will be resolved.
         * \param   portNr      The port number to resolve.
         * \param   isServer    The flag, indicating whether it should resolve for client
         *                      or server socket. The server socket supposed to bind.
         * \return  Returns true if succeeded to resolve name.
         **/
        bool resolveAddress( const char * hostName, unsigned short portNr, bool isServer );

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
        String        mIpAddr;
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
    extern AREG_API const SOCKETHANDLE          InvalidSocketHandle         /*= INVALID_SOCKET*/; // invalid socket descriptor
    /**
     * \brief   NESocket::InvalidSocket
     *          Constant, identifying invalid port number
     **/
    extern AREG_API const unsigned short        InvalidPort                 /*= 0*/;
    /**
     * \brief   NESocket::LocalHost
     *          Constant, identifying local host
     **/
    extern AREG_API const char * const          LocalHost                   /*= "localhost"*/;
    /**
     * \brief   NESocket::MAXIMUM_LISTEN_QUEUE_SIZE
     *          Constant, identifying maximum number of listeners in the queue.
     *          Used by server socket when set to listen connection.
     **/
    extern AREG_API const int                   MAXIMUM_LISTEN_QUEUE_SIZE   /*= SOMAXCONN*/;
    /**
     * \brief   NESocket::DEFAULT_SEGMENT_SIZE
     *          The default size of segment when sends or receives data.
     **/
    extern AREG_API const int                   DEFAULT_SEGMENT_SIZE        /*= 16384*/; // 1460;

//////////////////////////////////////////////////////////////////////////
// NESocket namespace functions
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   NESocket::isSocketHandleValid
     *          Checks socket descriptor and returns true if it not equal to InvalidSocketHandle.
     * \param   hSocket     The socket descriptor to check
     **/
    AREG_API bool isSocketHandleValid( SOCKETHANDLE hSocket );

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
    AREG_API SOCKETHANDLE clientSocketConnect( const NESocket::InterlockedValue & peerAddr );

    /**
     * \brief   NESocket::clientSocketConnect
     *          Creates client TCP/IP socket and connect to specified remote host name and port number.
     *          The host name can be either numeric IP-address or human readable host name to resolve.
     *          If passed out_socketAddr pointer is not NULL, on output it will contain readable
     *          numeric IP-address with dots and port number (IP-address like "123.45.678.90"
     * \param   hostName    The host name or IP_address of remote server to connect.
     * \param   portNr      The port number to connect. This should be valid port number.
     * \return  Returns valid socket descriptor, if could create socket and connect to remote peer.
     *          Otherwise, it returns NESocket::InvalidSocketHandle value.
     **/
    AREG_API SOCKETHANDLE clientSocketConnect( const char * hostName, unsigned short portNr, NESocket::InterlockedValue * out_socketAddr = NULL );

    /**
     * \brief   NESocket::serverSocketConnect
     *          Creates server TCP/IP socket and binds to specified server address.
     *          Before accepting any connection, the socket should be set for listening.
     * \param   peerAddr    The object containing host IP-address and port number of server.
     * \return  Returns valid socket descriptor, if could create socket and bind to specified address.
     *          Otherwise, it returns NESocket::InvalidSocketHandle value.
     **/
    AREG_API SOCKETHANDLE serverSocketConnect( const NESocket::InterlockedValue & peerAddr );

    /**
     * \brief   NESocket::serverSocketConnect
     *          Creates server TCP/IP socket and binds to specified host name and port number.
     *          Before accepting any connection, the socket should be set for listening.
     *          The host name can be either numeric IP-address or human readable host name to resolve.
     *          If passed out_socketAddr pointer is not NULL, on output it will contain readable
     *          numeric IP-address with dots and port number (IP-address like "123.45.678.90"
     * \param   hostName    The host name or server IP-address to bind.
     * \param   portNr      The port number to bind. This should be valid port number.
     * \return  Returns valid socket descriptor, if could create socket and bind to specified address.
     *          Otherwise, it returns NESocket::InvalidSocketHandle value.
     **/
    AREG_API SOCKETHANDLE serverSocketConnect( const char * hostName, unsigned short portNr, NESocket::InterlockedValue * out_socketAddr = NULL );

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
     *                          Or NULL if server has no accepted connection yet.
     * \param   entriesCount    The number of entries in specified accepted list. 
     *                          Or Zero if server has no accepted connection yet.
     * \param   out_socketAddr  If not NULL and new connection is accepted, on output this will contain
     *                          the IP address and port number of new accepted connection.
     *                          Note:   the data will be valid only in case of new accepted connections.
     *                                  In all other cases there will be no valid data.
     * \return  If succeeds to accept connection, returns valid accepted socket descriptor.
     *          Otherwise, if server socket is not valid anymore, returns NESocket::InvalidSocketHandle.
     **/
    AREG_API SOCKETHANDLE serverAcceptConnection( SOCKETHANDLE serverSocket, const SOCKETHANDLE * masterList, int entriesCount, NESocket::InterlockedValue * out_socketAddr = NULL );

    /**
     * \brief   NESocket::getMaxSendSize
     *          Calculated the maximum number of package size in bytes, which can be sent.
     *          This value may vary by protocol.
     * \param   hSocket     The valid socket descriptor to retrieve value.
     **/
    AREG_API int getMaxSendSize( SOCKETHANDLE hSocket );

    /**
     * \brief   NESocket::getMaxReceiveSize
     *          Calculated the maximum number of package size in bytes, which can be received.
     *          This value may vary by protocol.
     * \param   hSocket     The valid socket descriptor to retrieve value.
     **/
    AREG_API int getMaxReceiveSize( SOCKETHANDLE hSocket );

    /**
     * \brief   NESocket::sendData
     *          Send data to specified socket. The passed socket descriptor should be valid.
     *          If blockMaxSize value is negative, it will retrieve the maximum number of package size
     *          to send at once and try to send data.
     * \param   hSocket         The valid socket descriptor to send data.
     * \param   dataBuffer      The pointer to data buffer, which should be sent.
     * \param   dataLength      The length of buffer in bytes.
     * \param   blockMaxSize    The maximum size of package in bytes to sent at once.
     *                          If negative value, it will first retrieve value and sent data.
     * \return  If succeeds, returns number of bytes sent.
     *          If failles, returns negative number.
     *          Returns zero if buffer is empty and nothing to sent.
     **/
    AREG_API int sendData( SOCKETHANDLE hSocket, const unsigned char * dataBuffer, int dataLength, int blockMaxSize = NECommon::DefaultSize );

    /**
     * \brief   NESocket::receiveData
     *          Receives data on specified socket. The passed socket descriptor should be valid.
     *          If blockMaxSize value is negative, it will retrieve the maximum number of package size
     *          to receive at once.
     * \param   hSocket         The valid socket descriptor to receive data.
     * \param   dataBuffer      The pointer to data buffer, which should be filled.
     * \param   dataLength      The length of buffer in bytes.
     * \param   blockMaxSize    The maximum size of package in bytes to receive at once.
     *                          If negative value, it will first retrieve value and start receive data.
     * \return  If succeeds, returns number of bytes received.
     *          If failles, returns negative number. Failure might happen if opposite side closes connection.
     *          In case of failure, the specified socket should be closed.
     *          Returns zero if buffer is empty and nothing to receive.
     **/
    AREG_API int receiveData( SOCKETHANDLE hSocket, unsigned char * dataBuffer, int dataLength, int blockMaxSize  = NECommon::DefaultSize );

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
     * \brief   NESocket::remainDataRead
     *          Returns the remaining amount of data in bytes to read in a single receive call.
     * \param   hSocket     The socket handle to check
     * \return  Returns the remaining amount of data in bytes to receive in single call.
     **/
    AREG_API unsigned int remainDataRead( SOCKETHANDLE hSocket );

    /**
     * \brief   Checks and returns socket alive state.
     * \param   hSocket     The socked handle to check.
     * \return  Returns true if specified socket is alive and is not closed.
     **/
    AREG_API bool isSocketAlive( SOCKETHANDLE hSocket );

    /**
     * \brief   Checks and returns number of bytes remain to read from socket buffer.
     *          Returns negative value if socket is invalid.
     * \param   hSocket     The socket handle to check.
     * \return  Returns number of bytes available to read from specified socket buffer.
     **/
    AREG_API int pendingRead( SOCKETHANDLE hSocket );

}   // namespace NESocket end

//////////////////////////////////////////////////////////////////////////
// NESocket::InterlockedValue class inline function
//////////////////////////////////////////////////////////////////////////

inline bool NESocket::InterlockedValue::isValid( void ) const
{
    return ((mIpAddr.isEmpty() == false) && (mPortNr != NESocket::InvalidPort));
}

inline const String & NESocket::InterlockedValue::getHostAddress( void ) const
{
    return mIpAddr;
}

inline unsigned short NESocket::InterlockedValue::getHostPort( void ) const
{
    return mPortNr;
}

inline void NESocket::InterlockedValue::resetAddress( void )
{
    mIpAddr = String::EmptyString;
    mPortNr = NESocket::InvalidPort;
}

#endif  // AREG_BASE_NESOCKET_HPP
