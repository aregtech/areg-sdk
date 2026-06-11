#ifndef AREG_BASE_SOCKETCLIENT_HPP
#define AREG_BASE_SOCKETCLIENT_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/SocketClient.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform Client Socket class declaration
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/Socket.hpp"

#include <string_view>

namespace areg {

//////////////////////////////////////////////////////////////////////////
// SocketClient class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Client socket for connecting to remote hosts via TCP/IP; sends and receives data. Create
 *          before communication and close when no longer needed.
 **/
class AREG_API SocketClient   : public    Socket
{
//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    SocketClient() = default;

    /**
     * \brief   Initialization constructor. Resolves host name and port number and sets the socket
     *          address if resolution succeeds. If hostName is empty, resolves to localhost.
     *
     * \param   hostName    Host name or IP-address of remote server to connect.
     * \param   portNr      Port number of remote server to connect.
     **/
    SocketClient(const String& hostName, uint16_t portNr);

    SocketClient( const char* hostName, uint16_t portNr );

    /**
     * \brief   Initialization constructor. Sets the specified address as the remote server address
     *          to connect.
     *
     * \param   remoteAddress       Address of remote host to connect.
     **/
    SocketClient( const areg::SocketAddress & remoteAddress );

    virtual ~SocketClient() = default;

public:
/************************************************************************/
// Socket overrides
/************************************************************************/

    /**
     * \brief   Creates a socket descriptor and connects to the specified remote host and port.
     *
     * \param   hostName    The name of host to connect.
     * \param   portNr      The valid port number to connect.
     * \return  Returns true if operation succeeded.
     **/
    bool create(const String& hostName, uint16_t portNr) override;

    bool create(const char * hostName, uint16_t portNr ) override;

    /**
     * \brief   Creates a socket descriptor and connects to the remote address previously set in the
     *          socket address.
     *
     * \return  Returns true if operation succeeded.
     **/
    bool create() override;

    /**
     * \brief   Creates a socket file descriptor without connecting. Use connect_to() to establish the
     *          connection (e.g. from a dedicated I/O thread). mAddress must be set beforehand.
     *
     * \return  Returns true if the socket descriptor was created.
     **/
    bool create_fd();

    /**
     * \brief   Blocking TCP connect to the stored remote address. Must be called after create_fd().
     *          On failure the socket is closed and invalidated. On success applies TCP_NODELAY.
     *
     * \return  Returns true if the connection was established.
     **/
    bool connect_to();

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE( SocketClient );
};

} // namespace areg
#endif  // AREG_BASE_SOCKETCLIENT_HPP
