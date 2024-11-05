#ifndef AREG_IPC_SERVERCONNECTIONBASE_HPP
#define AREG_IPC_SERVERCONNECTIONBASE_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/ipc/ServerConnectionBase.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform Server Connection class declaration.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"

#include "areg/base/Containers.hpp"
#include "areg/base/SynchObjects.hpp"
#include "areg/base/SocketServer.hpp"
#include "areg/base/SocketAccepted.hpp"
#include "areg/component/NEService.hpp"

//////////////////////////////////////////////////////////////////////////
// ServerConnectionBase class declaration.
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The Server Socket is used to accept connection from remote clients,
 *          send and receive data. Before accepting connections,
 *          sending or receiving any data, the socket should be created,
 *          set for listening and wait for incoming connection.
 *          When connection from client is accepted, the server specifies
 *          unique cookie for accepted client. As soon as connection is
 *          accepted, the server can start to send and receive data.
 *          Connection accepting, sending and receiving data are running
 *          in blocking mode. For this reason, it makes sens to run all these
 *          functionalities in separate threads.
 *          Server socket is using only TCP/IP connection. All other types
 *          and protocols are out of scope of this class and are not considered.
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
    using MapSocketToObject 	= TEMap<SOCKETHANDLE, SocketAccepted>;

    /**
     * \brief   The container of socket handles where the keys are cookie values.
     **/
    using MapCookieToSocket		= TEMap<ITEM_ID, SOCKETHANDLE>;

    /**
     * \brief   The container of cookie values where the keys are socket handles.
     **/
    using MapSocketToCookie		= TEMap<SOCKETHANDLE, ITEM_ID>;

    /**
     * \brief   The list of accepted sockets.
     **/
    using ListSockets			= TEArrayList<SOCKETHANDLE>;

    /**
     * \brief   The size of master list to listen sockets for incoming messages.
     **/
    static constexpr int    MASTER_LIST_SIZE    { 64 };

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Creates instance of object with invalid socket object.
     *          Before sending or receiving data, the socket should be created
     *          and bound to socket address.
     **/
    ServerConnectionBase( void );

    /**
     * \brief   Creates instance of object with invalid socket object.
     *          Before sending or receiving data, the socket should be created
     *          and bound to specified local IP-address and port.
     *          When instantiated, it will resolved passed host
     *          name and port number. If succeeded to resolve,
     *          it will set resolved IP-address and port number
     *          as socket address. If passed hostName is nullptr,
     *          it resolve connection for local host.
     * \param   hostName    Host name or IP-address of server.
     * \param   portNr      Port number of server.
     **/
    ServerConnectionBase( const String & hostName, unsigned short portNr );

    /**
     * \brief   Creates instance of object with invalid socket object.
     *          Before sending or receiving data, the socket should be created
     *          and bound to host and port. Specified remoteAddress will be set as server address.
     * \param   serverAddress   The address of server socket.
     **/
    ServerConnectionBase( const NESocket::SocketAddress & serverAddress );

    /**
     * \brief   Destructor.
     **/
    virtual ~ServerConnectionBase( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Return Socket Address object.
     **/
    inline const NESocket::SocketAddress & getAddress( void ) const;

    /**
     * \brief   Sets Socket Address. If hostName is not IP-address, it will
     *          try to resolve first then set. The isServer parameter is needed
     *          to resolve address either for server or for client.
     *          For accepted sockets this call plays no role, because the
     *          the address automatically is resolved when accepting connection.
     * \param   hostName    Host name or IP-address to set. If name is specified,
     *                      first it will be resolved to get IP-address.
     * \param   portNr      Valid port number of socket connection.
     * \return  Returns true if succeeded to resolve and set Socket Address.
     **/
    inline bool setAddress( const String & hostName, unsigned short portNr );

    /**
     * \brief   Sets socket address. The address should be either invalid
     *          or already resolved with IP-address.
     * \param   newAddress  The new address to set.
     **/
    inline void setAddress( const NESocket::SocketAddress & newAddress );

    /**
     * \brief   Returns true if existing socket descriptor is valid.
     *          The function is not checking socket descriptor validation.
     **/
    inline bool isValid( void ) const;

    /**
     * \brief   Returns handle of socket.
     **/
    inline SOCKETHANDLE getSocketHandle( void ) const;

    /**
     * \brief   Returns true if connection with specified socket is accepted.
     * \param   connection      The socket to check connection acceptance.
     **/
    inline bool isConnectionAccepted( SOCKETHANDLE connection ) const;

    /**
     * \brief   Returns cookie of client connection set by server.
     *          Cookie is checked when sending or receiving data as
     *          source or target in Remote Buffer.
     * \param   clientSocket    Accepted client socket connection
     **/
    inline ITEM_ID getCookie( const SocketAccepted & clientSocket ) const;

    /**
     * \brief   Returns cookie of client connection set by server.
     *          Cookie is checked when sending or receiving data as
     *          source or target in Remote Buffer.
     * \param   socketHandle    Socket handle of accepted client connection
     **/
    inline ITEM_ID getCookie( SOCKETHANDLE socketHandle ) const;

    /**
     * \brief   Returns accepted socket object, which is matching passed cookie.
     *          If client cookie is valid, the return accepted socket object is valid.
     *          Otherwise, it is invalid accepted client socket object.
     * \param   clientCookie    The client cookie. Should be valid cookie
     * \return  If there is registered accepted client socket object, which matches client cookie,
     *          the returned object is valid accepted client object. Otherwise, the object is invalid.
     **/
    inline SocketAccepted getClientByCookie(const ITEM_ID & clientCookie ) const;

    /**
     * \brief   Returns accepted socket object, with same unique socket handle.
     *          If connection was accepted, the returned object is valid.
     *          Otherwise, it returns invalid accepted client socket object.
     * \param   clientSocket    The client socket. Should be valid handle
     * \return  If there is registered accepted client socket object, which matches socket handle,
     *          the returned object is valid accepted client object. Otherwise, the object is invalid.
     **/
    inline SocketAccepted getClientByHandle( SOCKETHANDLE clientSocket ) const;

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Before listening and accepting connection from clients,
     *          call this method to create new socket descriptor and bind
     *          socket to specified host name and port number.
     * \param   hostName    The name of host to bind.
     * \param   portNr      The valid port number to bind.
     * \return  Returns true if operation succeeded.
     **/
    bool createSocket( const String & hostName, unsigned short portNr );

    /**
     * \brief   Before listening and accepting connection from clients,
     *          call this method to create new socket descriptor and bind
     *          socket to exiting local IP-address and port number.
     *          Both, socket IP-address and port number should be already set.
     * \return  Returns true if operation succeeded.
     **/
    bool createSocket( void );

    /**
     * \brief   Closes existing socket.
     *          The call will disconnect all accepted connections.
     **/
    void closeSocket( void );

    /**
     * \brief   Call to place server socket in a state in which it is listening for an incoming connection.
     *          To accept connections on server side, firs socket should be created, which is bound to a
     *          local address. A backlog for incoming connections is specified with listen, and the length
     *          of pending connections are specified in maxQueueSize parameter. Then the connections are accepted.
     * \param   maxQueueSize    The maximum size of the socket queue in the list.
     **/
    bool serverListen( int maxQueueSize = NESocket::MAXIMUM_LISTEN_QUEUE_SIZE );

    /**
     * \brief   Call to wait for connection event. Function is blocking call until connection
     *          event is not triggered. Once connection event happens, the function returns
     *          valid socket handle of connected event. On output out_addrNewAccepted parameter
     *          changes only if new client connection is accepted. In all other cases, when
     *          client send data or closes socket, this parameter will not be changed, and
     *          the method does not distinguish whether client socket is closed or send data.
     *          The connection event is fired when new client is connected, when client
     *          is sending data or client closes connection.
     * \param   out_addrNewAccepted On output, if new connection is accepted, this parameter
     *                              contain address of new accepted socket. In all other cases,
     *                              when client sends data or close socket, this parameter
     *                              remains unchanged.
     * \return  If function succeeds, the function returns valid socket handle. For new connections,
     *          out_addrNewAccepted parameter contains address of accepted socket.
     *          If function fails, returns invalid socket handle.
     **/
    SOCKETHANDLE waitForConnectionEvent(NESocket::SocketAddress & out_addrNewAccepted);

    /**
     * \brief   Call to accept connection. Nothing will happen if connection was already accepted.
     *          For new connections, on output out_connection parameter will have accepted state.
     * \param[out]  clientConnection    Connection to accept. If object is valid, on output this will
     *                                  be in accepted state.
     **/
    bool acceptConnection( SocketAccepted & clientConnection );

    /**
     * \brief   Call to close client connection.
     * \param   clientConnection    The client to close connection.
     **/
    void closeConnection( SocketAccepted & clientConnection );

    /**
     * \brief   Call to close connection by cookie.
     * \param   cookie      The cookie of client set by routing service.
     **/
    void closeConnection(const ITEM_ID & cookie);

    /**
     * \brief   Sets socket in the read-only mode, i.e. no send message is possible anymore.
     * \param   clientConnection    The connected client socket to set in read-only mode.
     * \return  Returns true if operation succeeds.
     **/
    inline bool disableSend( const SocketAccepted & clientConnection );

    /**
     * \brief   Sets socket in the write-only mode, i.e. no receive message is possible anymore.
     * \param   clientConnection    The connected client socket to set in write-only mode.
     * \return  Returns true if operation succeeds.
     **/
    inline bool disableReceive( const SocketAccepted & clientConnection );

    /**
     * \brief   Sets all sockets in the read-only mode, i.e. no send message is possible anymore.
     **/
    inline void disableSend( void );

    /**
     * \brief   Sets all sockets in the write-only more, i.e. no receive message is possible anymore.
     **/
    inline void disableReceive( void );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   The instance of server socket connection, which accepts connections
     **/
    SocketServer        mServerSocket;
    /**
     * \brief   The cookie value generator, counter.
     **/
    ITEM_ID             mCookieGenerator;

#if defined(_MSC_VER) && (_MSC_VER > 1200)
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
    /**
     * \brief   The list of accepted sockets.
     **/
    ListSockets         mMasterList;
#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(default: 4251)
#endif  // _MSC_VER

    /**
     * \brief   Synchronization object for data sharing
     **/
    mutable ResourceLock    mLock;
//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    DECLARE_NOCOPY_NOMOVE( ServerConnectionBase );
};

//////////////////////////////////////////////////////////////////////////
// ServerConnectionBase class inline functions
//////////////////////////////////////////////////////////////////////////

inline bool ServerConnectionBase::setAddress(const String & hostName, unsigned short portNr)
{
    Lock lock(mLock);
    return mServerSocket.setAddress(hostName, portNr, true);
}

inline void ServerConnectionBase::setAddress( const NESocket::SocketAddress & newAddress )
{
    Lock lock(mLock);
    mServerSocket.setAddress(newAddress);
}

inline const NESocket::SocketAddress & ServerConnectionBase::getAddress( void ) const
{
    Lock lock(mLock);
    return mServerSocket.getAddress();
}

inline bool ServerConnectionBase::isValid( void ) const
{
    Lock lock(mLock);
    return mServerSocket.isValid();
}

inline SOCKETHANDLE ServerConnectionBase::getSocketHandle( void ) const
{
    Lock lock(mLock);
    return mServerSocket.getHandle();
}

inline bool ServerConnectionBase::isConnectionAccepted( SOCKETHANDLE connection ) const
{
    Lock lock(mLock);
    return mAcceptedConnections.contains(connection);
}

inline ITEM_ID ServerConnectionBase::getCookie(const SocketAccepted & clientSocket) const
{
    return getCookie(clientSocket.getHandle());
}

inline ITEM_ID ServerConnectionBase::getCookie(SOCKETHANDLE socketHandle) const
{
    Lock lock( mLock );

    MapSocketToCookie::MAPPOS pos = mSocketToCookie.find( socketHandle );
    return (mSocketToCookie.isValidPosition(pos) ? mSocketToCookie.valueAtPosition(pos) : NEService::COOKIE_UNKNOWN );
}

inline SocketAccepted ServerConnectionBase::getClientByCookie(const ITEM_ID & clientCookie) const
{
    Lock lock( mLock );
    MapCookieToSocket::MAPPOS pos = mCookieToSocket.find(clientCookie);
    return (mCookieToSocket.isValidPosition(pos) ? getClientByHandle( mCookieToSocket.valueAtPosition(pos) ) : SocketAccepted());
}

inline SocketAccepted ServerConnectionBase::getClientByHandle(SOCKETHANDLE clientSocket) const
{
    Lock lock( mLock );
    MapSocketToObject::MAPPOS pos = mAcceptedConnections.find(clientSocket);
    return (mAcceptedConnections.isValidPosition(pos) ? mAcceptedConnections.getAt(clientSocket) : SocketAccepted());
}

inline bool ServerConnectionBase::disableSend( const SocketAccepted & clientConnection )
{
    return clientConnection.disableSend();
}

inline bool ServerConnectionBase::disableReceive( const SocketAccepted & clientConnection )
{
    return clientConnection.disableReceive();
}

inline void ServerConnectionBase::disableSend( void )
{
    for ( MapSocketToObject::MAPPOS pos = mAcceptedConnections.firstPosition( ); mAcceptedConnections.isValidPosition( pos ); pos = mAcceptedConnections.nextPosition( pos ) )
    {
        mAcceptedConnections.valueAtPosition( pos ).disableSend( );
    }
}

inline void ServerConnectionBase::disableReceive( void )
{
    for ( MapSocketToObject::MAPPOS pos = mAcceptedConnections.firstPosition( ); mAcceptedConnections.isValidPosition( pos ); pos = mAcceptedConnections.nextPosition( pos ) )
    {
        mAcceptedConnections.valueAtPosition( pos ).disableReceive( );
    }
}

#endif  // AREG_IPC_SERVERCONNECTIONBASE_HPP
