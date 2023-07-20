#ifndef AREG_IPC_PRIVATE_CLIENTCONNECTION_HPP
#define AREG_IPC_PRIVATE_CLIENTCONNECTION_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/ipc/private/ClientConnection.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform Client Connection class declaration
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/ipc/SocketConnectionBase.hpp"

#include "areg/base/SocketClient.hpp"

//////////////////////////////////////////////////////////////////////////
// ClientConnection class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The Client Socket is used to connect to remote host server,
 *          send and receive data. Before sending or receiving any data,
 *          the socket should be created and as soon as connection is not needed,
 *          it should be closed.
 *          Client socket is using only TCP/IP connection. All other types
 *          and protocols are out of scope of this class and are not considered.
 **/
class ClientConnection   : private   SocketConnectionBase
{
//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Creates instance with invalid socket object. Before sending
     *          or receiving data, the socket should be created and connected to remote host.
     **/
    ClientConnection( void );

    /**
     * \brief   Creates instance of object with invalid socket object. Before sending
     *          or receiving data, the socket should be created and connected to remote host.
     *          When instantiated, it resolves host name and port number. If succeeded to resolve,
     *          it sets resolved IP-address and port number as socket address. If passed hostName is nullptr,
     *          it resolve connection for localhost.
     * \param   hostName    Host name or IP-address of remote server to connect. If nullptr, the connection
     *                      is setup for localhost.
     * \param   portNr      Port number of remote server to connect, should not be invalid port.
     **/
    ClientConnection( const String & hostName, unsigned short portNr );

    /**
     * \brief   Creates instance of object with invalid socket object. Before sending
     *          or receiving data, the socket should be created and connected to remote host.
     *          Specified remoteAddress is a remote server address to connect.
     * \param   remoteAddress   Address of remote host to connect.
     **/
    ClientConnection( const NESocket::SocketAddress & remoteAddress );

    /**
     * \brief   Destructor.
     **/
    virtual ~ClientConnection( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns cookie of client connection set by server.
     *          Cookie is checked when sending or receiving data as
     *          source or target in Remote Buffer.
     **/
    ITEM_ID getCookie( void ) const;

    /**
     * \brief   Sets cookie of client connection set by server
     *          Cookie is checked when sending or receiving data as
     *          source or target in Remote Buffer.
     **/
    void setCookie( ITEM_ID newCookie );

    /**
     * \brief   Return Socket Address object.
     **/
    const NESocket::SocketAddress & getAddress( void ) const;

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
    bool setAddress( const String & hostName, unsigned short portNr );

    /**
     * \brief   Sets socket address. The address should be either invalid
     *          or already resolved with IP-address.
     * \param   newAddress  The new address to set.
     **/
    void setAddress( const NESocket::SocketAddress & newAddress );

    /**
     * \brief   Returns true if existing socket descriptor is valid.
     *          The function is not checking socket descriptor validation.
     **/
    bool isValid( void ) const;

    /**
     * \brief   Returns the socket object.
     **/
    Socket & getSocket( void );

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Before sending or receiving any data from remote host,
     *          call this method to create new socket descriptor and 
     *          connect to specified remote host and port.
     * \param   hostName    The name of host to connect.
     * \param   portNr      The valid port number to connect.
     * \return  Returns true if operation succeeded.
     **/
    bool createSocket( const String & hostName, unsigned short portNr );

    /**
     * \brief   Before sending or receiving any data from remote host,
     *          call this method to create new socket descriptor and 
     *          connects to existing socket address. The remote host address
     *          and port number should be already set in socket address.
     * \return  Returns true if operation succeeded.
     **/
    bool createSocket( void );

    /**
     * \brief   Closes existing socket.
     *          The call will disconnect from remote server.
     **/
    void closeSocket( void );

public:
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
     * \param   in_message  The instance of buffer to send. The checksum number of Remote Buffer object
     *                      will be checked before sending. If checksum is invalid, the data will not be sent.
     * \return  Returns length in bytes of data in Remote Buffer sent to remote host. 
     *          Returns negative number if socket is not valid of failed to send.
     *          Returns zero, if checksum in Remote Buffer was not validated or Remote Buffer object is empty.
     **/
    int sendMessage( const RemoteMessage & in_message ) const;

    /**
     * \brief   If socket is valid, receives data using existing socket connection and returns length in bytes
     *          of data in Remote Buffer. And returns negative number if either socket is invalid,
     *          or failed to receive data from remote host. If Remote Buffer data is empty or checksum is,
     *          not matching, it will return zero.
     *          Note:   The returned value of received data (used data length) will be different of total buffer length.
     *          Note:   If received Remote Buffer was empty, on output out_message in invalid.
     *          Note:   The call is blocking and method will not return until all data are not received
     *                  or if data receiving fails.
     * \param   out_message The instance of Remote Buffer to receive data. The checksum number of Remote Buffer object
     *                      will be checked after receiving data. If checksum is invalid, the data will invalidated and dropped.
     * \return  Returns length in bytes of data in Remote Buffer received from remote host.
     *          Returns negative number if socket is not valid of failed to send.
     *          Returns zero, if checksum in Remote Buffer was not validated or data in Remote Buffer object is empty.
     **/
    int receiveMessage( RemoteMessage & out_message ) const;

    /**
     * \brief   Sets socket in read-only more, i.e. no send message is possible anymore.
     * \return  Returns true if operation succeeds.
     **/
    bool disableSend( void );

    /**
     * \brief   Sets socket in write-only more, i.e. no receive message is possible anymore.
     * \return  Returns true if operation succeeds.
     **/
    bool disableReceive( void );

    /**
     * \brief   Called by connection service to start connection with routing service.
     * \return  Returns true if succeeded to connect to routing service.
     **/
    bool requestConnectServer( void );

    /**
     * \brief   Called by connection service to disconnect from routing service.
     *          Once disconnected, all services receive disconnect notification and stop communication
     *          via routing service.
     * \return  Returns true if there was an active connection and the system succeeded to disconnect.
     *          Returns false, if there was no active connection.
     **/
    bool requestDisconnectServer( void );

    /**
     * \brief   Generates client disconnected message to broadcast.
     **/
    RemoteMessage getDisconnectMessage( void ) const;

//////////////////////////////////////////////////////////////////////////
// Member variables.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The client connection socket
     **/
    SocketClient    mClientSocket;

    /**
     * \brief   Client connection cookie
     **/
    ITEM_ID         mCookie;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    DECLARE_NOCOPY_NOMOVE( ClientConnection );
};

//////////////////////////////////////////////////////////////////////////
// ClientConnection class inline functions
//////////////////////////////////////////////////////////////////////////

inline bool ClientConnection::setAddress(const String & hostName, unsigned short portNr)
{
    return mClientSocket.setAddress(hostName, portNr, false);
}

inline ITEM_ID ClientConnection::getCookie( void ) const
{
    return mCookie;
}

inline void ClientConnection::setCookie( ITEM_ID newCookie )
{
    mCookie = newCookie;
}

inline const NESocket::SocketAddress & ClientConnection::getAddress( void ) const
{
    return mClientSocket.getAddress();
}

inline void ClientConnection::setAddress( const NESocket::SocketAddress & newAddress )
{
    mClientSocket.setAddress(newAddress);
}

inline bool ClientConnection::isValid( void ) const
{
    return mClientSocket.isValid();
}

inline bool ClientConnection::disableSend( void )
{
    return mClientSocket.disableSend( );
}

inline bool ClientConnection::disableReceive( void )
{
    return mClientSocket.disableReceive();
}

inline Socket & ClientConnection::getSocket( void )
{
    return mClientSocket;
}

inline int ClientConnection::sendMessage(const RemoteMessage & in_message) const
{
    return SocketConnectionBase::sendMessage(in_message, mClientSocket);
}

inline int ClientConnection::receiveMessage(RemoteMessage & out_message) const
{
    return SocketConnectionBase::receiveMessage(out_message, mClientSocket);
}

#endif  // AREG_IPC_PRIVATE_CLIENTCONNECTION_HPP
