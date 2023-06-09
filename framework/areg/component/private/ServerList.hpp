#ifndef AREG_COMPONENT_PRIVATE_SERVERLIST_HPP
#define AREG_COMPONENT_PRIVATE_SERVERLIST_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/component/private/ServerList.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, Server List declaration
 *
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/TEHashMap.hpp"
#include "areg/component/private/ServerInfo.hpp"
#include "areg/component/private/ClientList.hpp"

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
using ServerListBase    = TEHashMap<ServerInfo, ClientList>;

/**
 * \brief   Server List is a Hash Map class containing information
 *          of Server Info and Client List. The Key entry of Server List
 *          is Server Info and the Values are Client List. For every
 *          Stub of Service Interface, it is created List of Clients, to
 *          track the connection of Stub server and Proxies as clients.
 *          The controlling of server and connected clients done via
 *          registration of client(s).
 **/
class ServerList  : public ServerListBase
{

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Creates empty Server List.
     **/
    ServerList( void ) = default;
    /**
     * \brief   Destructor
     **/
    ~ServerList( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns true if server with requested address is registered.
     **/
    bool isServerRegistered( const StubAddress & server ) const;

    /**
     * \brief   Registers requested client by given address in the list.
     *          The function creates Client Info object, looks up whether
     *          there is registered Server existing in the system. If found,
     *          inserts Client Info in existing Client List. If did not find,
     *          creates new Server Info with Undefined state and inserts
     *          Client Info in the list.
     *          The function returns registered Client Info object.
     * \param   whichClient     The address of Proxy-client to register in the list.
     * \param   serviceType     Type of service. If ServiceAny, it will depend whether 
     *                          Proxy address is local or remote
     * \return  Returns registered Client Info object.
     **/
    const ServerInfo & registerClient( const ProxyAddress & whichClient, ClientInfo & out_client );

    /**
     * \brief   Unregisters specified Proxy client from Server List.
     *          If operation succeeded, on output the out_client contains information
     *          of Client and returns the number of clients of Service Interface.
     *          If function returns -1, either there is no such registered client
     *          in the system, or the Stub server is in Undefined state and
     *          there are no more clients waiting for Service Interface.
     * \param   whichClient The address of Proxy client to unregister.
     * \param   out_client  On output, this will contain Client information
     * \return  If returns -1, either client was not registered, or the list
     *          of clients is empty and the server is in Undefined state.
     *          Any other value specifies number of existing client related
     *          with Service Interface.
     **/
    ServerInfo unregisterClient( const ProxyAddress & whichClient, ClientInfo & out_client );

    /**
     * \brief   Registers specified Stub Server in the list, if the address is valid, it sets Server state
     *          connected. On output, out_clientWaitingList will contain list of Client Info waiting
     *          for server connection. The function returns updated and registered Server Info object.
     * \param   server              The Stub server address to register. 
     *                              If valid, it sets Server Info state to connected.
     * \param   serviceType         Type of service. If ServiceAny, it will depend whether Server stub
     *                              address is local or remote
     * \param   out_waitingClients  On output, contains list of Client Info objects waiting for
     *                              Server to be connected.
     * \return  Returns updated and registered Server Info object.
     **/
    const ServerInfo & registerServer( const StubAddress & addrStub, ClientList & out_clinetList );

    /**
     * \brief   Unregisters specified Stub Server from the list and returns related
     *          to Service Interface Client Info object list.
     * \param   whichServer The address of Stub Server object to unregister
     * \return  Returns related to Service Interface Client Info object list.
     **/
    ServerInfo unregisterServer( const StubAddress & whichServer, ClientList & out_clinetList );

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Returns the state of specified Stub server. The state is either
     *          Undefined, if Server is not registered yet, or Connected, if
     *          Server already registered and runs.
     * \param   whichServer The address of Stub Server to check the state.
     * \return  Returns ServerInfo::SERVER_UNDEFINED state if specified Server
     *          is not registered yet, or returns ServerInfo::SERVER_REGISTERED
     *          if Server is registered and runs.
     **/
    NEService::eServiceConnection getServerState( const StubAddress & whichServer ) const;

    /**
     * \brief   Returns Client Info List related with specified server.
     * \param   whichServer The address of Stub Server to get list.
     * \return  Returns Client Info List related with specified server.
     *          If list is empty, there is no Client connected to server yet.
     **/
    const ClientList & getClientList( const StubAddress & whichServer ) const;

    /**
     * \brief   Searches the server info object for given client.
     *          Returns valid pointer if found any. Otherwise, returns null.
     * \param   whichClient     The instance of client information object to extract data for search.
     * \return  Returns valid pointer if found registered server. Otherwise, it returns null.
     **/
    const ServerInfo * findClientServer( const ProxyAddress & whichClient ) const;

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   Find server component by client proxy address.
     **/
    MAPPOS findServer(const ProxyAddress& whichClient) const;

    /**
     * \brief   Find server component by service address.
     **/
    MAPPOS findServer(const StubAddress& whichServer) const;

    /**
     * \brief   Find server component by server info.
     **/
    MAPPOS findServer(const ServerInfo& server) const;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    DECLARE_NOCOPY_NOMOVE( ServerList );
};

#endif  // AREG_COMPONENT_PRIVATE_SERVERLIST_HPP
