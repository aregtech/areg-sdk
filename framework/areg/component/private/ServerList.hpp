#ifndef AREG_COMPONENT_PRIVATE_SERVERLIST_HPP
#define AREG_COMPONENT_PRIVATE_SERVERLIST_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        areg/component/private/ServerList.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Server List declaration
 *
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/HashMap.hpp"
#include "areg/component/private/ServerInfo.hpp"
#include "areg/component/private/ClientList.hpp"
namespace areg {

/************************************************************************
 * Dependencies
 ************************************************************************/
class StubAddress;
class ProxyAddress;

//////////////////////////////////////////////////////////////////////////
// ServerList declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Server List helper class.
 **/
using ServerListBase    = HashMap<ServerInfo, ClientList>;

/**
 * \brief   Hash map storing server-to-client associations; tracks connections between service
 *          providers and consumers.
 **/
class ServerList  : public ServerListBase
{

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    ServerList() = default;
    ~ServerList() = default;

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns true if a server with the specified address is registered.
     *
     * \param   server      The stub address to check.
     **/
    [[nodiscard]]
    bool is_server_registered( const StubAddress & server ) const;

    /**
     * \brief   Registers a client by proxy address; creates or updates server info and returns the
     *          registered client info.
     *
     * \param   whichClient     The proxy address of the client to register.
     * \param[out] out_client      On output, contains the information of the registered client.
     * \return  Returns the registered or updated server info.
     **/
    const ServerInfo & register_consumer( const ProxyAddress & whichClient, ClientInfo & out_client );

    /**
     * \brief   Unregisters a client and returns the client info and remaining client count.
     *
     * \param   whichClient         The proxy address of the client to unregister.
     * \param[out] out_client       On output, contains the unregistered client information.
     * \return  Returns the number of remaining clients; -1 if client was not registered or server
     *          is in Undefined state with no clients.
     **/
    ServerInfo unregister_consumer( const ProxyAddress & whichClient, ClientInfo & out_client );

    /**
     * \brief   Registers a service provider and returns the list of waiting clients.
     *
     * \param   addrStub            The stub address to register; valid address sets server to Connected.
     * \param[out] out_clinetList   On output, contains the list of clients waiting for the server connection.
     * \return  Returns the registered server info.
     **/
    const ServerInfo & register_provider( const StubAddress & addrStub, ClientList & out_clinetList );

    /**
     * \brief   Unregisters a service provider and returns the list of its connected clients.
     *
     * \param   whichServer         The stub address of the server to unregister.
     * \param[out] out_clinetList   On output, contains the list of clients previously connected to the server.
     * \return  Returns the server info being unregistered.
     **/
    ServerInfo unregister_provider( const StubAddress & whichServer, ClientList & out_clinetList );

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Returns the connection state of the specified server.
     *
     * \param   whichServer     The stub address of the server to query.
     * \return  Returns Undefined if server is not registered, or Connected if server is running.
     **/
    areg::ServiceConnectionState server_state( const StubAddress & whichServer ) const;

    /**
     * \brief   Returns the list of clients connected to the specified server.
     *
     * \param   whichServer     The stub address of the server.
     * \return  Returns the client list; empty if no clients are connected.
     **/
    const ClientList & client_list( const StubAddress & whichServer ) const;

    /**
     * \brief   Searches for the server info associated with the specified client.
     *
     * \param   whichClient     The proxy address to search for.
     * \return  Returns a valid pointer if the server is found; null otherwise.
     **/
    const ServerInfo * find_client_server( const ProxyAddress & whichClient ) const;

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   Finds the server entry position for the given client proxy address.
     *
     * \param   whichClient     The proxy address to search for.
     **/
    MAPPOS find_provider(const ProxyAddress& whichClient) const;

    /**
     * \brief   Finds the server entry position for the given stub address.
     *
     * \param   whichServer     The stub address to search for.
     **/
    MAPPOS find_provider(const StubAddress& whichServer) const;

    /**
     * \brief   Finds the server entry position for the given server info.
     *
     * \param   server      The server info to search for.
     **/
    MAPPOS find_provider(const ServerInfo& server) const;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE( ServerList );
};

} // namespace areg
#endif  // AREG_COMPONENT_PRIVATE_SERVERLIST_HPP
