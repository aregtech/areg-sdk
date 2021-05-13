#ifndef AREG_IPC_PRIVATE_SERVERCONNECTION_HPP
#define AREG_IPC_PRIVATE_SERVERCONNECTION_HPP

/************************************************************************
 * \file        areg/ipc/private/ServerConnection.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform Server Connection class declaration.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/ipc/private/SocketConnectionBase.hpp"

#include "areg/base/ESynchObjects.hpp"
#include "areg/base/SocketServer.hpp"
#include "areg/base/SocketAccepted.hpp"
#include "areg/base/TEHashMap.hpp"
#include "areg/base/TEArrayList.hpp"

//////////////////////////////////////////////////////////////////////////
// ServerConnection class declaration.
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
class AREG_API ServerConnection   : private   SocketConnectionBase
{
    /**
     * \brief   The container of accepted socket objects where the keys are socket handle.
     **/
    typedef TEHashMapImpl<const SOCKETHANDLE, const SocketAccepted &>                                                     MapSocketToObjectImpl;
    typedef TEHashMap<SOCKETHANDLE, SocketAccepted, const SOCKETHANDLE, const SocketAccepted &, MapSocketToObjectImpl>  MapSocketToObject;

    /**
     * \brief   The container of socket handles where the keys are cookie values.
     **/
    typedef TEHashMapImpl<ITEM_ID, SOCKETHANDLE>                                                MapCookieToSocketImpl;
    typedef TEHashMap<ITEM_ID, SOCKETHANDLE, ITEM_ID, SOCKETHANDLE, MapCookieToSocketImpl>      MapCookieToSocket;

    /**
     * \brief   The container of cookie values where the keys are socket handles.
     **/
    typedef TEHashMapImpl<SOCKETHANDLE, ITEM_ID>                                                MapSocketToCookieImpl;
    typedef TEHashMap<SOCKETHANDLE, ITEM_ID, SOCKETHANDLE, ITEM_ID, MapSocketToCookieImpl>      MapSocketToCookie;

    /**
     * \brief   The list of accepted sockets.
     **/
    typedef TEArrayList<SOCKETHANDLE, const SOCKETHANDLE>                                       ListSockets;

    /**
     * \brief   The size of master list to listen sockets for incoming messages.
     **/
    static const int        MASTER_LIST_SIZE        /*= 64*/;

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Creates instance of object with invalid socket object.
     *          Before sending or receiving data, the socket should be created
     *          and bound to socket address.
     **/
    ServerConnection( void );

    /**
     * \brief   Creates instance of object with invalid socket object.
     *          Before sending or receiving data, the socket should be created 
     *          and bound to specified local IP-address and port.
     *          When instantiated, it will resolved passed host
     *          name and port number. If succeeded to resolve,
     *          it will set resolved IP-address and port number
     *          as socket address. If passed hostName is NULL,
     *          it resolve connection for local host.
     * \param   hostName    Host name or IP-address of server.
     * \param   portNr      Port number of server.
     **/
    ServerConnection( const char * hostName, unsigned short portNr );

    /**
     * \brief   Creates instance of object with invalid socket object.
     *          Before sending or receiving data, the socket should be created 
     *          and bound to host and port. Specified remoteAddress will be set as server address.
     * \param   remoteAddress   Address of server.
     **/
    ServerConnection( const NESocket::InterlockedValue & serverAddress );

    /**
     * \brief   Destructor.
     **/
    virtual ~ServerConnection( void );

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Return Socket Address object.
     **/
    inline const NESocket::InterlockedValue & getAddress( void ) const;

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
    inline bool setAddress( const char * hostName, unsigned short portNr );

    /**
     * \brief   Sets socket address. The address should be either invalid
     *          or already resolved with IP-address.
     * \param   newAddress  The new address to set.
     **/
    inline void setAddress( const NESocket::InterlockedValue & newAddress );

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
    ITEM_ID getCookie( const SocketAccepted & clientSocket ) const;

    /**
     * \brief   Returns cookie of client connection set by server.
     *          Cookie is checked when sending or receiving data as
     *          source or target in Remote Buffer.
     * \param   socketHandle    Socket handle of accepted client connection
     **/
    ITEM_ID getCookie( SOCKETHANDLE socketHandle ) const;

    /**
     * \brief   Returns accepted socket object, which is matching passed cookie.
     *          If client cookie is valid, the return accepted socket object is valid.
     *          Otherwise, it is invalid accepted client socket object.
     * \param   clientCookie    The client cookie. Should be valid cookie
     * \return  If there is registered accepted client socket object, which matches client cookie,
     *          the returned object is valid accepted client object. Otherwise, the object is invalid.
     **/
    SocketAccepted getClientByCookie( ITEM_ID clientCookie ) const;

    /**
     * \brief   Returns accepted socket object, with same unique socket handle.
     *          If connection was accepted, the returned object is valid.
     *          Otherwise, it returns invalid accepted client socket object.
     * \param   clientSocket    The client socket. Should be valid handle
     * \return  If there is registered accepted client socket object, which matches socket handle,
     *          the returned object is valid accepted client object. Otherwise, the object is invalid.
     **/
    SocketAccepted getClientByHandle( SOCKETHANDLE clientSocket ) const;

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
    bool createSocket( const char * hostName, unsigned short portNr );

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
     * \param   maxQueueSize    
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
    SOCKETHANDLE waitForConnectionEvent(NESocket::InterlockedValue & out_addrNewAccepted);

    /**
     * \brief   Call to accept connection. Nothing will happen if connection was already accepted.
     *          For new connections, on output out_connection parameter will have accepted state.
     * \param   out_connection  Connection to accept. If object is valid, on output this will 
     *                          be in accepted state.
     **/
    bool acceptConnection( SocketAccepted & clientConnection );

    /**
     * \brief   Call to reject connection. When rejected, the socket connection will be closed and
     *          no more data will be accepted from connection.
     * \param   out_connection  Connection to reject.
     **/
    void rejectConnection( SocketAccepted & clientConnection );

    /**
     * \brief   Call to close client connection.
     **/
    void closeConnection( SocketAccepted & clientConnection );

    /**
     * \brief   Call to close all existing accepted connections
     **/
    void closeAllConnections( void );

    /**
     * \brief   If socket is valid, sends data using existing socket connection and returns length in bytes
     *          of data in Remote Buffer. And returns negative number if either socket is invalid,
     *          or failed to send data to remote host. No data will be sent, if Remote Buffer is empty.
     *          Before sending data, the method will check and validate existing checksum in buffer
     *          structure. And if checksum is invalid, the data will not be sent to remote target.
     *          If checksum is invalid, the returned value is zero.
     *          Note:   The returned value of sent data (used data length) will be different of total buffer length.
     *          Note:   If Remote Buffer is empty, nothing will be sent.
     *          Note:   The call is blocking and method will not return until all data are not sent
     *                  or if data sending fails.
     *          Note:   Check and set checksum before sending data.
     * \param   in_message      The instance of buffer to send. The checksum number of Remote Buffer object
     *                          will be checked before sending. If checksum is invalid, the data will not be sent.
     * \param   clientSocket    The accepted socket object
     * \return  Returns length in bytes of data in Remote Buffer sent to remote host. 
     *          Returns negative number if socket is not valid of failed to send.
     *          Returns zero, if checksum in Remote Buffer was not validated or Remote Buffer object is empty.
     **/
    inline int sendMessage( const RemoteMessage & in_message, const SocketAccepted & clientSocket ) const;

    /**
     * \brief   If socket is valid, receives data using existing socket connection and returns length in bytes
     *          of data in Remote Buffer. And returns negative number if either socket is invalid,
     *          or failed to receive data from remote host. If Remote Buffer data is empty or checksum is,
     *          not matching, it will return zero.
     *          Note:   The returned value of received data (used data length) will be different of total buffer length.
     *          Note:   If received Remote Buffer was empty, on output out_message in invalid.
     *          Note:   The call is blocking and method will not return until all data are not received
     *                  or if data receiving fails.
     * \param   out_message     The instance of Remote Buffer to receive data. The checksum number of Remote Buffer object
     *                          will be checked after receiving data. If checksum is invalid, the data will invalidated and dropped.
     * \param   clientSocket    The accepted socket object
     * \return  Returns length in bytes of data in Remote Buffer received from remote host.
     *          Returns negative number if socket is not valid of failed to send.
     *          Returns zero, if checksum in Remote Buffer was not validated or data in Remote Buffer object is empty.
     **/
    inline int receiveMessage( RemoteMessage & out_message, const SocketAccepted & clientSocket ) const;

    /**
     * \brief   If socket is valid, sends data using existing socket connection and returns length in bytes
     *          of data in Remote Buffer. And returns negative number if either socket is invalid,
     *          or failed to send data to remote host. No data will be sent, if Remote Buffer is empty.
     *          Before sending data, the method will check and validate existing checksum in buffer
     *          structure. And if checksum is invalid, the data will not be sent to remote target.
     *          If checksum is invalid, the returned value is zero.
     *          Note:   The returned value of sent data (used data length) will be different of total buffer length.
     *          Note:   If Remote Buffer is empty, nothing will be sent.
     *          Note:   The call is blocking and method will not return until all data are not sent
     *                  or if data sending fails.
     *          Note:   Check and set checksum before sending data.
     * \param   in_message      The instance of buffer to send. The checksum number of Remote Buffer object
     *                          will be checked before sending. If checksum is invalid, the data will not be sent.
     * \param   clientCookie    The cookie number of accepted socket connection
     * \return  Returns length in bytes of data in Remote Buffer sent to remote host. 
     *          Returns negative number if socket is not valid of failed to send.
     *          Returns zero, if checksum in Remote Buffer was not validated or Remote Buffer object is empty.
     **/
    inline int sendMessage( const RemoteMessage & in_message, ITEM_ID clientCookie ) const;

    /**
     * \brief   If socket is valid, receives data using existing socket connection and returns length in bytes
     *          of data in Remote Buffer. And returns negative number if either socket is invalid,
     *          or failed to receive data from remote host. If Remote Buffer data is empty or checksum is,
     *          not matching, it will return zero.
     *          Note:   The returned value of received data (used data length) will be different of total buffer length.
     *          Note:   If received Remote Buffer was empty, on output out_message in invalid.
     *          Note:   The call is blocking and method will not return until all data are not received
     *                  or if data receiving fails.
     * \param   out_message     The instance of Remote Buffer to receive data. The checksum number of Remote Buffer object
     *                          will be checked after receiving data. If checksum is invalid, the data will invalidated and dropped.
     * \param   clientCookie    The cookie number of accepted socket connection
     * \return  Returns length in bytes of data in Remote Buffer received from remote host.
     *          Returns negative number if socket is not valid of failed to send.
     *          Returns zero, if checksum in Remote Buffer was not validated or data in Remote Buffer object is empty.
     **/
    inline int receiveMessage( RemoteMessage & out_message, ITEM_ID clientCookie ) const;

    /**
     * \brief   Sets socket in read-only more, i.e. no send message is possible anymore.
     * \return  Returns true if operation succeeds.
     **/
    inline bool disableSend( const SocketAccepted & clientConnection );

    /**
     * \brief   Sets socket in write-only more, i.e. no receive message is possible anymore.
     * \return  Returns true if operation succeeds.
     **/
    inline bool disableReceive( const SocketAccepted & clientConnection );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The instance of server socket connection, which accepts connections
     **/
    SocketServer      mServerSocket;
    /**
     * \brief   The cookie value generator, counter.
     **/
    ITEM_ID             mCookieGenerator;

#if _MSC_VER
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
#if _MSC_VER
    #pragma warning(default: 4251)
#endif  // _MSC_VER

    /**
     * \brief   Synchronization object for data sharing
     **/
    mutable ResourceLock   mLock;
//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    ServerConnection( const ServerConnection & );
    const ServerConnection & operator = ( const ServerConnection & );
};

//////////////////////////////////////////////////////////////////////////
// ServerConnection class inline functions
//////////////////////////////////////////////////////////////////////////

inline bool ServerConnection::setAddress(const char * hostName, unsigned short portNr)
{
    Lock lock(mLock);
    return mServerSocket.setAddress(hostName, portNr, true);
}

inline void ServerConnection::setAddress( const NESocket::InterlockedValue & newAddress )
{
    Lock lock(mLock);
    mServerSocket.setAddress(newAddress);
}

inline const NESocket::InterlockedValue & ServerConnection::getAddress( void ) const
{
    Lock lock(mLock);
    return mServerSocket.getAddress();
}

inline bool ServerConnection::isValid( void ) const
{
    Lock lock(mLock);
    return mServerSocket.isValid();
}

inline SOCKETHANDLE ServerConnection::getSocketHandle( void ) const
{
    Lock lock(mLock);
    return mServerSocket.getHandle();
}

inline bool ServerConnection::isConnectionAccepted( SOCKETHANDLE connection ) const
{
    Lock lock(mLock);
    return (mAcceptedConnections.find(connection) != NULL);
}

inline int ServerConnection::sendMessage(const RemoteMessage & in_message, const SocketAccepted & clientSocket) const
{
    return SocketConnectionBase::sendMessage(in_message, clientSocket);
}

inline int ServerConnection::sendMessage(const RemoteMessage & in_message, ITEM_ID clientCookie) const
{
    return SocketConnectionBase::sendMessage(in_message, getClientByCookie(clientCookie) );
}

inline int ServerConnection::receiveMessage(RemoteMessage & out_message, const SocketAccepted & clientSocket) const
{
    return SocketConnectionBase::receiveMessage(out_message, clientSocket);
}

inline int ServerConnection::receiveMessage(RemoteMessage & out_message, ITEM_ID clientCookie) const
{
    return SocketConnectionBase::receiveMessage(out_message,getClientByCookie(clientCookie));
}

inline bool ServerConnection::disableSend( const SocketAccepted & clientConnection )
{
    return clientConnection.disableSend();
}

inline bool ServerConnection::disableReceive( const SocketAccepted & clientConnection )
{
    return clientConnection.disableReceive();
}

#endif  // AREG_IPC_PRIVATE_SERVERCONNECTION_HPP
