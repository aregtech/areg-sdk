#ifndef AREG_BASE_CESOCKETSERVER_HPP
#define AREG_BASE_CESOCKETSERVER_HPP

/************************************************************************
 * \file        areg/src/base/CESocketServer.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform Server Socket class declaration.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/src/base/GEGlobal.h"
#include "areg/src/base/CESocket.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class CESocketAccepted;

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
//////////////////////////////////////////////////////////////////////////
// CESocketServer class declaration.
//////////////////////////////////////////////////////////////////////////
class AREG_API CESocketServer   : public    CESocket
{
public:
    /**
     * \brief   Default constructor. Creates instance of object
     *          with invalid socket object. Before sending
     *          or receiving data, the socket should be created
     *          and bound to socket address.
     **/
    CESocketServer( void );

    /**
     * \brief   Initialization constructor. Creates instance of object
     *          with invalid socket object. Before sending
     *          or receiving data, the socket should be created 
     *          and bound to specified local IP-address and port.
     *          When instantiated, it will resolved passed host
     *          name and port number. If succeeded to resolve,
     *          it will set resolved IP-address and port number
     *          as socket address. If passed hostName is NULL,
     *          it resolve connection for local host.
     * \param   hostName    Host name or IP-address of server.
     * \param   portNr      Port number of server.
     **/
    CESocketServer( const char * hostName, unsigned short portNr );

    /**
     * \brief   Initialization constructor. Creates instance of object
     *          with invalid socket object. Before sending
     *          or receiving data, the socket should be created 
     *          and bound to host and port.
     *          Specified remoteAddress will be set as server address.
     * \param   remoteAddress   Address of server.
     **/
    CESocketServer( const NESocket::CEInterlockedValue & serverAddress );

    /**
     * \brief   Destructor.
     **/
    virtual ~CESocketServer( void );

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// CESocket overrides
/************************************************************************/

    /**
     * \brief   Before listening and accepting connection from clients, 
     *          call this method to create new socket descriptor and bind 
     *          socket to specified host name and port number.
     * \param   hostName    The name of host to bind.
     * \param   portNr      The valid port number to bind.
     * \return  Returns true if operation succeeded.
     **/
    virtual bool CreateSocket( const char * hostName, unsigned short portNr );

    /**
     * \brief   Before listening and accepting connection from clients, 
     *          call this method to create new socket descriptor and bind 
     *          socket to exiting local IP-address and port number.
     *          Both, socket IP-address and port number should be already set.
     * \return  Returns true if operation succeeded.
     **/
    virtual bool CreateSocket( void );

    /**
     * \brief   Call to place server socket in a state in which it is listening for an incoming connection.
     *          To accept connections on server side, firs socket should be created, which is bound to a 
     *          local address. A backlog for incoming connections is specified with listen, and the length
     *          of pending connections are specified in maxQueueSize parameter. Then the connections are accepted.
     * \param   maxQueueSize    The maximum size of pending connection queue. If not positive value (0 or negative),
     *                          it will be use maximum NESocket::MAXIMUM_LISTEN_QUEUE_SIZE value
     **/
    virtual bool ListenConnection( int maxQueueSize );

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
     * \param   masterList          The master list of existing connections.
     * \param   entriesCount        The length of entries in master list.
     * \return  If function succeeds, the function returns valid socket handle. For new connections,
     *          out_addrNewAccepted parameter contains address of accepted socket. 
     *          If function fails, returns invalid socket handle.
     **/
    virtual SOCKETHANDLE WaitForConnectionEvent(NESocket::CEInterlockedValue & out_addrNewAccepted, const SOCKETHANDLE * masterList, int entriesCount);

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    CESocketServer( const CESocketServer & /*src*/ );
    const CESocketServer & operator = ( const CESocketServer & /*src*/ );
};

#endif  // AREG_BASE_CESOCKETSERVER_HPP
