#ifndef AREG_SERVICE_CONNECTIVITY_SERVERCONNECTION_HPP
#define AREG_SERVICE_CONNECTIVITY_SERVERCONNECTION_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        service/connectivity/ServerConnection.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform, service server connection class.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/ipc/ServerConnectionBase.hpp"
#include "areg/ipc/SocketConnectionBase.hpp"

#include "areg/base/SynchObjects.hpp"
#include "areg/base/SocketServer.hpp"
#include "areg/base/SocketAccepted.hpp"

//////////////////////////////////////////////////////////////////////////
// ServerConnection class declaration.
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The Server Socket is used to accept connection from remote clients,
 *          send and receive data. When connection from client is accepted,
 *          the server specifies unique cookie for accepted client. After
 *          connection is accepted, the client starts to send and receive data.
 *          Sending and receiving data are running in blocking mode. 
 *          To synchronize communication, there are send and receive threads
 *          are specified.
 **/
class ServerConnection  : public    ServerConnectionBase
                        , private   SocketConnectionBase
{
//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Creates instance of object with invalid socket object and
     *          sets the unique channel ID of the service in the system.
     *          Before sending or receiving data, the socket should be created
     *          and bound to socket address.
     * \brief   The unique channel ID of the service connectivity in the system.
     **/
    ServerConnection( ITEM_ID channelId );

    /**
     * \brief   Creates instance of object with invalid socket object.
     *          Before sending or receiving data, the socket should be created 
     *          and bound to specified local IP-address and port.
     *          When instantiated, it will resolved passed host
     *          name and port number. If succeeded to resolve,
     *          it will set resolved IP-address and port number
     *          as socket address. If passed hostName is nullptr,
     *          it resolve connection for local host.
     * \brief   The unique channel ID of the service connectivity in the system.
     * \param   hostName    Host name or IP-address of server.
     * \param   portNr      Port number of server.
     **/
    ServerConnection(ITEM_ID channelId, const char * hostName, unsigned short portNr );

    /**
     * \brief   Creates instance of object with invalid socket object.
     *          Before sending or receiving data, the socket should be created 
     *          and bound to host and port. Specified remoteAddress will be set as server address.
     * \brief   The unique channel ID of the service connectivity in the system.
     * \param   remoteAddress   Address of server.
     **/
    explicit ServerConnection(ITEM_ID channelId, const NESocket::SocketAddress & serverAddress );

    /**
     * \brief   Destructor.
     **/
    virtual ~ServerConnection( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns the unique service connectivity channel ID in the system.
     **/
    inline const ITEM_ID getChannelId(void) const;

    /**
     * \brief   Call to reject connection. When rejected, the socket connection will be closed and
     *          no more data will be accepted from connection.
     * \param   out_connection  Connection to reject.
     **/
    void rejectConnection( SocketAccepted & clientConnection );

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

//////////////////////////////////////////////////////////////////////////
// Hidden member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The unique channel ID of the service connectivity in the system.
     **/
    const ITEM_ID       mChannelId;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    ServerConnection(void) = delete;
    DECLARE_NOCOPY_NOMOVE( ServerConnection );
};

//////////////////////////////////////////////////////////////////////////
// ServerConnection class inline functions
//////////////////////////////////////////////////////////////////////////

inline const ITEM_ID ServerConnection::getChannelId(void) const
{
    return mChannelId;
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

#endif  // AREG_SERVICE_CONNECTIVITY_SERVERCONNECTION_HPP
