#ifndef AREG_COMPONENT_PRIVATE_CLIENTLIST_HPP
#define AREG_COMPONENT_PRIVATE_CLIENTLIST_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/ClientList.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Client List declaration
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/LinkedList.hpp"
#include "areg/component/private/ClientInfo.hpp"
namespace areg {

/************************************************************************
 * Dependencies
 ************************************************************************/
class ProxyAddress;
class ServerInfo;

using ClientListBase = LinkedList<ClientInfo>;

/**
 * \brief   Linked list of clients associated with a server stub; manages client connections and
 *          state transitions.
 **/
class ClientList    : public ClientListBase
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    ClientList() = default;

    ClientList( const ClientList & src ) = default;

    ClientList( ClientList && src ) noexcept = default;

    ~ClientList() = default;

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:

    ClientList & operator = ( const ClientList & src ) = default;

    ClientList & operator = ( ClientList && src ) noexcept = default;

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns true if a client with the specified proxy address exists in the list.
     *
     * \param   client      The proxy address to search for.
     **/
    [[nodiscard]]
    bool exist( const ProxyAddress & client ) const noexcept;

    /**
     * \brief   Returns the client info for the given proxy address.
     *
     * \param   whichClient     The proxy address to search for.
     **/
    [[nodiscard]]
    const ClientInfo & consumer( const ProxyAddress & whichClient ) const noexcept;

    /**
     * \brief   Registers a consumer in the list; returns existing client info if already registered,
     *          or creates and returns new entry.
     *
     * \param   whichClient     The proxy address of the client to register.
     * \param   server          The server info containing stub address and state.
     * \return  Returns the registered client info; increments instance count if already registered.
     **/
    const ClientInfo & register_consumer( const ProxyAddress & whichClient, const ServerInfo & server );

    /**
     * \brief   Unregisters a consumer from the list.
     *
     * \param   whichClient     The proxy address of the client to unregister.
     * \param[out] out_client      On output, contains information of the unregistered client;
     *                             unchanged if not found.
     * \return  Returns true if client was found and unregistered; false otherwise.
     **/
    bool unregister_consumer( const ProxyAddress & whichClient, ClientInfo & out_client );

    /**
     * \brief   Notifies all waiting clients that the server is available; updates their states.
     *
     * \param   whichServer         The server info indicating availability.
     * \param[out] out_clientList      On output, contains the list of connected clients.
     **/
    void provider_available( const ServerInfo & whichServer, ClientList & out_clientList );

    /**
     * \brief   Notifies all connected clients that the server is unavailable; updates their states.
     *
     * \param[out] out_clientList      On output, contains the list of disconnected clients.
     **/
    void provider_unavailable( ClientList & out_clientList );
};

} // namespace areg
#endif  // AREG_COMPONENT_PRIVATE_CLIENTLIST_HPP
