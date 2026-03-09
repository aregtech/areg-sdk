#ifndef AREG_BASE_SOCKETSERVER_HPP
#define AREG_BASE_SOCKETSERVER_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/SocketServer.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform Server Socket class declaration.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/Socket.hpp"

#include <string_view>
namespace areg {

/************************************************************************
 * Dependencies
 ************************************************************************/
class SocketAccepted;

//////////////////////////////////////////////////////////////////////////
// SocketServer class declaration.
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Server socket for accepting incoming TCP/IP connections, sending and receiving data.
 *          Supports blocking operations for connection acceptance and data transfer. Assigns unique
 *          cookies to accepted clients.
 **/
class AREG_API SocketServer   : public    Socket
{
public:
    SocketServer() = default;

    /**
     * \brief   Creates an instance and resolves the specified host name and port number. If
     *          hostName is empty, resolves to localhost.
     *
     * \param   hostName    Host name or IP address of the server. If empty, resolves to localhost.
     * \param   portNr      Port number of the server.
     **/
    SocketServer( const String& hostName, uint16_t portNr );

    SocketServer(const char* hostName, uint16_t portNr);

    /**
     * \brief   Creates an instance with the specified server address.
     *
     * \param   serverAddress       The address of the server socket.
     **/
    SocketServer( const areg::SocketAddress & serverAddress );

    virtual ~SocketServer() = default;

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// Socket overrides
/************************************************************************/

    /**
     * \brief   Creates a socket descriptor and binds it to the specified host name and port. Must
     *          be called before listening for connections.
     *
     * \param   hostName    The host name or IP address to bind the socket to.
     * \param   portNr      The port number to bind the socket to.
     * \return  Returns true if operation succeeded.
     **/
    bool create(const String& hostName, uint16_t portNr) override;

    bool create(const char * hostName, uint16_t portNr ) override;

    /**
     * \brief   Creates a socket descriptor and binds it to the previously configured host and port.
     *          Both address and port must be already set.
     *
     * \return  Returns true if operation succeeded.
     **/
    bool create() override;

    /**
     * \brief   Places the server socket in listening mode. Incoming connections are queued up to
     *          maxQueueSize.
     *
     * \param   maxQueueSize    The maximum number of pending connections to queue. If not positive,
     *                          uses areg::MAXIMUM_LISTEN_QUEUE_SIZE.
     * \return  Returns true if operation succeeded.
     **/
    virtual bool listen( int32_t maxQueueSize );

    /**
     * \brief   Waits for a connection event (new connection, data from client, or client
     *          disconnect). This is a blocking call.
     *
     * \param[in,out] addrAccepted      On successful new connection acceptance, contains the
     *                                  address of the accepted socket. Unchanged if client
     *                                  sends data or closes.
     * \param   masterList              Array of existing connection socket handles to monitor.
     * \param   entriesCount            The number of entries in the master list.
     * \return  Returns a valid socket handle if successful. If a new connection is accepted,
     *          addrAccepted contains the client address. Returns invalid handle if the
     *          function fails.
     **/
    virtual SOCKETHANDLE wait_connection_event(areg::SocketAddress & addrAccepted, const SOCKETHANDLE * masterList, int32_t entriesCount);

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE( SocketServer );
};

} // namespace areg
#endif  // AREG_BASE_SOCKETSERVER_HPP
