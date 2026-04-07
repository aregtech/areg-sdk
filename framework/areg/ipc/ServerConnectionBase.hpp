#ifndef AREG_IPC_SERVERCONNECTIONBASE_HPP
#define AREG_IPC_SERVERCONNECTIONBASE_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/ipc/ServerConnectionBase.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform Server Connection class declaration.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"

#include "areg/base/Containers.hpp"
#include "areg/base/SyncPrimitives.hpp"
#include "areg/base/SocketServer.hpp"
#include "areg/base/SocketAccepted.hpp"
#include "areg/component/ServiceDefs.hpp"
namespace areg {

//////////////////////////////////////////////////////////////////////////
// ServerConnectionBase class declaration.
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   TCP/IP server socket for accepting client connections, sending and receiving data in
 *          blocking mode. Manages multiple accepted connections with unique identifiers (cookies)
 *          and supports graceful shutdown.
 **/
class AREG_API ServerConnectionBase
{
//////////////////////////////////////////////////////////////////////////
// ServerConnectionBase class types and constants
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   The container of accepted socket objects where the keys are socket handle.
     **/
    using MapSocketToObject 	= OrderedMap<SOCKETHANDLE, SocketAccepted>;

    /**
     * \brief   The container of socket handles where the keys are cookie values.
     **/
    using MapCookieToSocket		= OrderedMap<ITEM_ID, SOCKETHANDLE>;

    /**
     * \brief   The container of cookie values where the keys are socket handles.
     **/
    using MapSocketToCookie		= OrderedMap<SOCKETHANDLE, ITEM_ID>;


//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    ServerConnectionBase();

    /**
     * \brief   Creates instance and resolves specified host name and port to socket address.
     *
     * \param   hostName    Host name or IP-address of server. If nullptr, resolves to localhost.
     * \param   portNr      Port number of server.
     **/
    ServerConnectionBase( const String & hostName, uint16_t portNr );

    /**
     * \brief   Creates instance with specified server socket address.
     *
     * \param   serverAddress       The address of server socket.
     **/
    ServerConnectionBase( const areg::SocketAddress & serverAddress );

    virtual ~ServerConnectionBase() = default;

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns the socket address object.
     **/
    [[nodiscard]]
    inline const areg::SocketAddress & address() const noexcept;

    /**
     * \brief   Resolves host name and sets socket address. For accepted sockets, address is
     *          resolved automatically on connection acceptance.
     *
     * \param   hostName    Host name or IP-address to set. If name is specified, first it will be
     *                      resolved to get IP-address.
     * \param   portNr      Valid port number of socket connection.
     * \return  Returns true if host name resolution and address setting succeeded.
     **/
    inline bool set_address( const String & hostName, uint16_t portNr );

    /**
     * \brief   Sets socket address from a resolved SocketAddress object.
     *
     * \param   newAddress      The new address to set.
     **/
    inline void set_address( const areg::SocketAddress & newAddress );

    /**
     * \brief   Returns true if socket descriptor is valid.
     **/
    [[nodiscard]]
    inline bool is_valid() const noexcept;

    /**
     * \brief   Returns the socket handle.
     **/
    [[nodiscard]]
    inline SOCKETHANDLE socket_handle() const noexcept;

    /**
     * \brief   Returns true if the specified socket connection has been accepted.
     *
     * \param   connection      The socket to check connection acceptance.
     **/
    [[nodiscard]]
    inline bool is_connection_accepted( SOCKETHANDLE connection ) const;

    /**
     * \brief   Returns the unique cookie identifier for an accepted client connection.
     *
     * \param   clientSocket    Accepted client socket connection.
     **/
    [[nodiscard]]
    inline ITEM_ID cookie( const SocketAccepted & clientSocket ) const;

    /**
     * \brief   Returns the unique cookie identifier for the socket handle of an accepted
     *          connection.
     *
     * \param   socketHandle    Socket handle of accepted client connection.
     **/
    [[nodiscard]]
    inline ITEM_ID cookie( SOCKETHANDLE socketHandle ) const;

    /**
     * \brief   Returns the accepted socket object matching the specified cookie, or invalid if not
     *          found.
     *
     * \param   clientCookie    The client cookie. Should be valid cookie.
     * \return  Returns valid SocketAccepted object if cookie matches; otherwise invalid.
     **/
    [[nodiscard]]
    inline SocketAccepted client_by_cookie(const ITEM_ID & clientCookie ) const;

    /**
     * \brief   Returns the accepted socket object matching the specified socket handle, or invalid
     *          if not found.
     *
     * \param   clientSocket    The client socket. Should be valid handle.
     * \return  Returns valid SocketAccepted object if socket handle matches; otherwise invalid.
     **/
    [[nodiscard]]
    inline SocketAccepted client_by_handle( SOCKETHANDLE clientSocket ) const;

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Creates socket descriptor and binds it to specified host name and port number.
     *
     * \param   hostName    The name of host to bind.
     * \param   portNr      The valid port number to bind.
     * \return  Returns true if socket creation and binding succeeded.
     **/
    bool create_socket( const String & hostName, uint16_t portNr );

    /**
     * \brief   Creates socket descriptor and binds it to the existing address. Address must be set
     *          before calling.
     *
     * \return  Returns true if socket creation and binding succeeded.
     **/
    bool create_socket();

    /**
     * \brief   Closes the socket and disconnects all accepted connections.
     **/
    void close_socket();

    /**
     * \brief   Places socket in listening state with specified backlog queue size.
     *
     * \param   maxQueueSize    The maximum size of the socket queue in the list.
     * \return  Returns true if listen setup succeeded.
     **/
    bool server_listen( int32_t maxQueueSize = areg::MAXIMUM_LISTEN_QUEUE_SIZE );

    /**
     * \brief   Blocking call that waits for connection event (new connection, data, or closure).
     *          Returns valid socket handle; address output parameter is updated only for new
     *          connections.
     *
     * \param[in,out] out_addrNewAccepted     On output, if new connection is accepted, this
     *                                        parameter contain address of new accepted socket. In
     *                                        all other cases, when client sends data or close
     *                                        socket, this parameter remains unchanged.
     * \return  Returns valid socket handle if successful; invalid if failed.
     **/
    SOCKETHANDLE wait_connection(areg::SocketAddress & out_addrNewAccepted);

    /**
     * \brief   Non-blocking variant of wait_connection. Returns immediately if no socket is ready.
     *          Use in a burst-drain loop after a blocking wait_connection() call returns a ready
     *          socket — processes all already-queued events before re-entering the blocking wait,
     *          reducing the number of epoll/kqueue/WSAPoll syscalls under high load.
     *
     * \param[in,out] out_addrNewAccepted    On output, receives the address of a newly accepted
     *                                       client. Unchanged when an existing client sends data.
     * \return  Valid socket handle if a socket is immediately ready;
     *          InvalidSocketHandle if nothing is ready (normal drain-loop termination);
     *          FailedSocketHandle on error or after reset().
     **/
    SOCKETHANDLE wait_connection_nowait(areg::SocketAddress & out_addrNewAccepted);

    /**
     * \brief   Accepts pending connection and updates clientConnection object to accepted state.
     *
     * \param[in,out] clientConnection    Connection to accept. If object is valid, on output this
     *                                    will be in accepted state.
     * \return  Returns true if connection was accepted.
     **/
    bool accept_connection( SocketAccepted & clientConnection );

    /**
     * \brief   Closes the specified client connection.
     *
     * \param   clientConnection    The client to close connection.
     **/
    void close_connection( SocketAccepted & clientConnection );

    /**
     * \brief   Closes connection identified by cookie.
     *
     * \param   cookie      The cookie of client set by routing service.
     **/
    void close_connection(const ITEM_ID & cookie);

    /**
     * \brief   Interrupts all accepted client connections by calling socket_interrupt() on each,
     *          without clearing the connection maps or closing any socket.
     *
     * Use this during graceful shutdown to unblock any thread stuck in send() or recv()
     * on an unresponsive client, while keeping the routing maps intact so that connection
     * cleanup (close_connection, close_socket) can proceed normally afterwards.
     **/
    void interrupt_connections() noexcept;

    /**
     * \brief   Sets socket to read-only mode, disabling message sending.
     *
     * \param   clientConnection    The connected client socket to set in read-only mode.
     * \return  Returns true if operation succeeds.
     **/
    inline bool disable_send( const SocketAccepted & clientConnection );

    /**
     * \brief   Sets socket to write-only mode, disabling message receiving.
     *
     * \param   clientConnection    The connected client socket to set in write-only mode.
     * \return  Returns true if operation succeeds.
     **/
    inline bool disable_receive( const SocketAccepted & clientConnection );

    /**
     * \brief   Configures the SO_SNDBUF and SO_RCVBUF sizes applied to every socket accepted by
     *          this server (overrides the compile-time default set by socket_configure()).
     *          Call before the first accept_connection() to take effect.
     *          Values of zero leave the compile-time defaults unchanged.
     *
     * \param   sendBuf     Desired SO_SNDBUF size in bytes (0 = keep default).
     * \param   recvBuf     Desired SO_RCVBUF size in bytes (0 = keep default).
     **/
    inline void set_socket_buffers(uint32_t sendBuf, uint32_t recvBuf) noexcept;

    /**
     * \brief   Sets all sockets to read-only mode, disabling message sending.
     **/
    inline void disable_send();

    /**
     * \brief   Sets all sockets to write-only mode, disabling message receiving.
     **/
    inline void disable_receive();

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   The instance of server socket connection, which accepts connections
     **/
    SocketServer        mServerSocket;
    /**
     * \brief   Persistent socket readiness monitor.  The server socket is registered in
     *          create_socket() and each accepted client is registered in accept_connection().
     *          On Linux, backed by epoll for O(1) readiness; poll() on other platforms.
     **/
    SocketMultiplexer   mMultiplexer;
    /**
     * \brief   The cookie value generator, counter.
     **/
    ITEM_ID             mCookieGenerator;

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable: 4251)
#endif  // _MSC_VER
    /**
     * \brief   The hash map of accepted sockets where the keys are socket handles.
     **/
    MapSocketToObject   mAcceptedConnections;
    /**
     * \brief   The hash map of sockets, where the keys are cookie values.
     **/
    MapCookieToSocket   mCookieToSocket;
    /**
     * \brief   The hash map of cookie values, where the key are socket handles.
     **/
    MapSocketToCookie   mSocketToCookie;
#if defined(_MSC_VER)
    #pragma warning(pop)
#endif  // _MSC_VER

    /**
     * \brief   SO_SNDBUF size applied to every accepted client socket.
     *          Initialized to SOCKET_SEND_BUFFER_SIZE; override via set_socket_buffers().
     **/
    uint32_t                mSockSendBuf;

    /**
     * \brief   SO_RCVBUF size applied to every accepted client socket.
     *          Initialized to SOCKET_RECV_BUFFER_SIZE; override via set_socket_buffers().
     **/
    uint32_t                mSockRecvBuf;

    /**
     * \brief   Synchronization object for data sharing
     **/
    mutable ResourceLock    mLock;
//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE( ServerConnectionBase );
};

//////////////////////////////////////////////////////////////////////////
// ServerConnectionBase class inline functions
//////////////////////////////////////////////////////////////////////////

inline bool ServerConnectionBase::set_address(const String & hostName, uint16_t portNr)
{
    Lock lock(mLock);
    return mServerSocket.set_address(hostName, portNr, true);
}

inline void ServerConnectionBase::set_address( const areg::SocketAddress & newAddress )
{
    Lock lock(mLock);
    mServerSocket.set_address(newAddress);
}

inline const areg::SocketAddress & ServerConnectionBase::address() const noexcept
{
    Lock lock(mLock);
    return mServerSocket.address();
}

inline bool ServerConnectionBase::is_valid() const noexcept
{
    Lock lock(mLock);
    return mServerSocket.is_valid();
}

inline SOCKETHANDLE ServerConnectionBase::socket_handle() const noexcept
{
    Lock lock(mLock);
    return mServerSocket.handle();
}

inline bool ServerConnectionBase::is_connection_accepted( SOCKETHANDLE connection ) const
{
    Lock lock(mLock);
    return mAcceptedConnections.contains(connection);
}

inline ITEM_ID ServerConnectionBase::cookie(const SocketAccepted & clientSocket) const
{
    return cookie(clientSocket.handle());
}

inline ITEM_ID ServerConnectionBase::cookie(SOCKETHANDLE socketHandle) const
{
    Lock lock( mLock );

    MapSocketToCookie::MAPPOS pos = mSocketToCookie.find( socketHandle );
    return (mSocketToCookie.is_valid_position(pos) ? mSocketToCookie.value_at(pos) : areg::COOKIE_UNKNOWN );
}

inline SocketAccepted ServerConnectionBase::client_by_cookie(const ITEM_ID & clientCookie) const
{
    Lock lock( mLock );
    MapCookieToSocket::MAPPOS pos = mCookieToSocket.find(clientCookie);
    return (mCookieToSocket.is_valid_position(pos) ? client_by_handle( mCookieToSocket.value_at(pos) ) : SocketAccepted());
}

inline SocketAccepted ServerConnectionBase::client_by_handle(SOCKETHANDLE clientSocket) const
{
    Lock lock( mLock );
    MapSocketToObject::MAPPOS pos = mAcceptedConnections.find(clientSocket);
    return (mAcceptedConnections.is_valid_position(pos) ? mAcceptedConnections.value_at(clientSocket) : SocketAccepted());
}

inline void ServerConnectionBase::set_socket_buffers(uint32_t sendBuf, uint32_t recvBuf) noexcept
{
    mSockSendBuf = (sendBuf > 0) ? sendBuf : mSockSendBuf;
    mSockRecvBuf = (recvBuf > 0) ? recvBuf : mSockRecvBuf;
}

inline bool ServerConnectionBase::disable_send( const SocketAccepted & clientConnection )
{
    return clientConnection.disable_send();
}

inline bool ServerConnectionBase::disable_receive( const SocketAccepted & clientConnection )
{
    return clientConnection.disable_receive();
}

inline void ServerConnectionBase::disable_send()
{
    for ( MapSocketToObject::MAPPOS pos = mAcceptedConnections.first_position( ); mAcceptedConnections.is_valid_position( pos ); pos = mAcceptedConnections.next_position( pos ) )
    {
        mAcceptedConnections.value_at( pos ).disable_send( );
    }
}

inline void ServerConnectionBase::disable_receive()
{
    for ( MapSocketToObject::MAPPOS pos = mAcceptedConnections.first_position( ); mAcceptedConnections.is_valid_position( pos ); pos = mAcceptedConnections.next_position( pos ) )
    {
        mAcceptedConnections.value_at( pos ).disable_receive( );
    }
}

} // namespace areg
#endif  // AREG_IPC_SERVERCONNECTIONBASE_HPP
