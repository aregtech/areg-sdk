#ifndef AREG_AREGEXTEND_SERVICE_CONNECTIONHANDLER_HPP
#define AREG_AREGEXTEND_SERVICE_CONNECTIONHANDLER_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        aregextend/service/ConnectionHandler.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Interface of service connection handler
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"

namespace areg {
    class SocketAccepted;
}

namespace areg::ext {

/************************************************************************
 * Dependencies
 ************************************************************************/

//////////////////////////////////////////////////////////////////////////
// ConnectionHandler class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Handles server-side connection acceptance and disconnection. Provides callbacks for
 *          validating new client connections, detecting lost connections, and disconnecting service
 *          providers and consumers.
 **/
class ConnectionHandler
{
//////////////////////////////////////////////////////////////////////////
// Protected constructor / destructor
//////////////////////////////////////////////////////////////////////////
protected:
    ConnectionHandler() = default;
    virtual ~ConnectionHandler() = default;

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// ConnectionHandler class overrides
/************************************************************************/

    /**
     * \brief   Call to check whether new client connection should be accepted or rejected. Once
     *          client is accepted, server will start to receive messages from client. Otherwise,
     *          connection with client is immediately closed and communication is stopped.
     *
     * \param   clientSocket    Accepted client socket object to check.
     * \return  Returns true if client connection can be accepted. To reject and close connection
     *          with client, the method should return false.
     **/
    [[nodiscard]]
    virtual bool can_accept_connection( const SocketAccepted & clientSocket ) = 0;

    /**
     * \brief   Triggered, when lost connection with client. Passed clientSocket parameter specifies
     *          client socket, which lost connection.
     *
     * \param   clientSocket    Client socket object, which lost connection.
     **/
    virtual void connection_lost( SocketAccepted & clientSocket ) = 0;

    /**
     * \brief   Triggered, when lost connection with client. Passed clientSocket parameter specifies
     *          client socket, which lost connection.
     **/
    virtual void connection_failure() = 0;

    /**
     * \brief   Called when need to disconnect and unregister all service providers and service
     *          consumers.
     **/
    virtual void disconnect_services() = 0;

    /**
     * \brief   Called after a new client socket has been physically accepted and registered with
     *          the server connection. Returning true signals that a dedicated per-client thread
     *          pair has taken ownership of the socket; ServerReceiveThread will NOT call
     *          receive_message() for this socket. Returning false (default) falls back to the
     *          legacy shared-thread receive path.
     *
     * \param   clientSocket    The newly accepted client socket (cookie already assigned).
     * \return  Returns true if a dedicated per-client thread pair was created and started for
     *          this socket. Returns false for the default shared-thread behavior.
     **/
    virtual bool on_client_accepted( SocketAccepted & /* clientSocket */ ) { return false; }

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE( ConnectionHandler );
};

} // namespace areg::ext

#endif  // AREG_AREGEXTEND_SERVICE_CONNECTIONHANDLER_HPP
